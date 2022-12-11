#include <stdio.h>
#ifdef WIN32
#include <conio.h>
#endif

//#define RTSPCLIENT_DLL
#define DEBUG
#ifdef LINUX
#undef RTSPCLIENT_DLL
#endif

#ifdef RTSPCLIENT_DLL
#pragma comment(lib, "RTSPClientDll.lib")
#include "RTSPClientDll.h"
#else
#pragma comment(lib, "RTSPClientLib.lib")
#include "RTSPClient.h"
#include "RTSPCommonEnv.h"
#endif

#ifdef WIN32
#include <windows.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#define mygetch	getch

#elif defined(LINUX)
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

int mygetch(void)
{
    struct termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
#endif

FILE *fp_dump = NULL;

#ifdef RTSPCLIENT_DLL
static void frameHandler(void *arg, DLL_RTP_FRAME_TYPE frame_type, __int64 timestamp, unsigned char *buf, int len)
#else
static void frameHandler(void *arg, RTP_FRAME_TYPE frame_type, int64_t timestamp, unsigned char *buf, int len)
#endif
{
	if (fp_dump && frame_type==0) {
		printf("buf len %d\n", len);
		fwrite(buf, len, 1, fp_dump);
	}
}

static void closeHandler(void* arg, int err, int result)
{
	printf("RTSP session disconnected, err : %d, result : %d", err, result);
}

int main(int argc, char *argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	int retry = 1;

	if (argc < 2)
		return -1;

#ifdef RTSPCLIENT_DLL
	rtspclient_set_debug_flag(DEBUG_FLAG_RTSP);
#else
	RTSPCommonEnv::SetDebugFlag(DEBUG_FLAG_RTSP);
#endif

#if 0
	char* strURL = "rtsp://127.0.0.1:8554/h264ESVideoTest";
	fp_dump = fopen("video.264", "wb");
#else
	char* strURL = argv[1];
	fp_dump = fopen("video.265", "wb");
#endif

#ifdef RTSPCLIENT_DLL
	void *rtspClient = rtspclient_new();
#else
	RTSPClient *rtspClient = new RTSPClient();
#endif

again:
#ifdef RTSPCLIENT_DLL
	if (rtspclient_open_url(rtspClient, strURL, 1, 2) == 0)
#else
	//STREAM_TYPE_UDP = 0, STREAM_TYPE_TCP = 1, STREAM_TYPE_MULTICAST = 2;
	if (rtspClient->openURL(strURL, 0, 2) == 0)
#endif
	{
		fwrite(rtspClient->videoExtraData(), rtspClient->videoExtraDataSize(), 1, fp_dump);

#ifdef RTSPCLIENT_DLL
		if (rtspclient_play_url(rtspClient, frameHandler, rtspClient, closeHandler, rtspClient) == 0)
#else
		if (rtspClient->playURL(NULL, rtspClient, closeHandler, rtspClient) == 0)
#endif
		{
			unsigned dataSize=1024*1024*4, outSize=0;
			uint8_t *data = new uint8_t[dataSize];
			uint8_t *outData = NULL;
			RTP_FRAME_TYPE  DataType;
			int64_t         dataTimestamp;
			char c;
			struct timespec timeout;
			timeout.tv_sec = time(NULL) + 2;
			timeout.tv_nsec = 0;
//			while (c = mygetch() != 'q')
			while(1)
			{
				rtspClient->setOutputData(data, dataSize);
				outData = rtspClient->getOutputDataTimeOut(outSize, DataType, dataTimestamp, &timeout);
				timeout.tv_sec = time(NULL) + 2;
				printf("buf len %d type %d time %ld\n", outSize, DataType, dataTimestamp);
				if (outData && DataType==0) {
					fwrite(outData, outSize, 1, fp_dump);
				}
			}
		}
	}	
exit:
#ifdef RTSPCLIENT_DLL
	rtspclient_close_url(rtspClient);
#else
	rtspClient->closeURL();
#endif

	printf("1closeURL\n");
	if (--retry >  0)
		goto again;

#ifdef RTSPCLIENT_DLL
	rtspclient_delete(rtspClient);
#else
	delete rtspClient;
#endif
	printf("2closeURL\n");

	if (fp_dump) fclose(fp_dump);
	printf("3closeURL\n");

	return 0;
}
