# vim: syntax=cmake
# ----------------------------------------------------------------------------
# mpp built-in unit test case
# ----------------------------------------------------------------------------
# macro for adding mpp sub-module unit test

project(rtsp_client)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_INSTALL_PREFIX /usr)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Os -Wall")

OPTION(RELEASE "Release" OFF)

include_directories(./Common ./Sock
    ./OS_Common
    ./Util
    ./RTSPClient/Common ./RTSPClient/RTCP ./RTSPClient/RTP ./RTSPClient/RTSP
                    )

add_library(rtsp_client SHARED
    Common/RTSPCommon.cpp  Common/RTSPCommonEnv.cpp
    OS_Common/Event.cpp  OS_Common/Mutex.cpp  OS_Common/MySemaphore.cpp  OS_Common/Thread.cpp
    Sock/MySock.cpp  Sock/MySock.h  Sock/SockCommon.cpp  Sock/TaskScheduler.cpp
    Util/Base64.cpp  Util/our_md5.c  Util/our_md5hl.c  Util/util.cpp
    RTSPClient/Common/BitVector.cpp  RTSPClient/Common/DigestAuthentication.cpp
    RTSPClient/RTCP/BasicHashTable.cpp  RTSPClient/RTCP/HashTable.cpp  RTSPClient/RTCP/OutPacketBuffer.cpp  RTSPClient/RTCP/RTCP.cpp  RTSPClient/RTCP/rtcp_from_spec.c  RTSPClient/RTCP/RTCPInstance.cpp
    RTSPClient/RTP/AC3RTPSource.cpp   RTSPClient/RTP/H265RTPSource.cpp  RTSPClient/RTP/MPEG4ESRTPSource.cpp       RTSPClient/RTP/RTPPacketBuffer.cpp
RTSPClient/RTP/H264RTPSource.cpp  RTSPClient/RTP/JPEGRTPSource.cpp  RTSPClient/RTP/MPEG4GenericRTPSource.cpp  RTSPClient/RTP/RTPSource.cpp
    RTSPClient/RTSP/MediaSession.cpp  RTSPClient/RTSP/RTSPClient.cpp
    )

target_compile_definitions(rtsp_client PRIVATE -DLINUX)
