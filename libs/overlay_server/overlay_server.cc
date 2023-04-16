#include "overlay_server.h"
#include "overlay_proto.h"
#include <core/logger.h>
#include <core/throw_if.h>
#include <core/types.h>
#include <iostream>
#include <string>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#endif

namespace DmxEnttecNode {

static const LogModule LM_OVERLAY {"OVERLAY"};

OverlayServer::OverlayServer(const Config& config, EventLoop& loop, IOverlayHandler& handler)
	: mConfig(config)
	, mEventLoop(loop)
	, mHandler(handler)
{
}

OverlayServer::~OverlayServer()
{
	if (mRunning)
	{
		closesocket(mSocket);
		WSACleanup();
	}
}

void OverlayServer::Start()
{
	StartServer(1234);
	mListenSocketPollHandle = mEventLoop.AddPoller([this]() { PollSocket(); });
	mMetricsHandle = mEventLoop.AddTimer(5s, [this]() { ReportMetrics(); });
}

void OverlayServer::StartServer(int port)
{
	LOG(LL_INFO, LM_OVERLAY, "starting overlay server on port %d..", port);

	int error = WSAStartup(0x0202, &mWsaData);
	THROW_IF(error, "unable to open WSA");

	mSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mSocket == INVALID_SOCKET)
	{
		WSACleanup();
		THROW_IF(true, "unable to open socket");
	}

	bool is_blocking = false;
	u_long flags = is_blocking ? 0 : 1;
	ioctlsocket(mSocket, FIONBIO, &flags);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons (port);
	addr.sin_addr.s_addr = htonl (INADDR_ANY);

	if (bind(mSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(mSocket);
		WSACleanup();
		THROW_IF(true, "unable to bind to socket");
	}

	LOG(LL_INFO, LM_OVERLAY, "server started on port %d", port);
	mRunning = true;
}

void OverlayServer::PollSocket()
{
	static int sClientAddrLen = sizeof(mClientAddr);
	int result = recvfrom(mSocket, mRecvBuffer, sizeof(mRecvBuffer), 0, (sockaddr*)&mClientAddr, &sClientAddrLen);
	if (result == SOCKET_ERROR)
	{
		return;
	}

	LOG(LL_DEBUG, LM_OVERLAY, "received message from %s", inet_ntoa(mClientAddr.sin_addr));
	OnData(mRecvBuffer, sizeof(mRecvBuffer));
}

void OverlayServer::OnData(const char* data, int len)
{
	static int sOverlayMessageSize = sizeof(OverlayIdl::OverlayMessage);
	if (len < sOverlayMessageSize)
	{
		return;
	}

	int protocolId = 0;
	std::memcpy(&protocolId, data, sizeof(uint32_t));
	if (protocolId != OVERLAY_PROTOCOL_ID)
	{
		return;
	}

	OverlayIdl::OverlayMessage message;
	std::memcpy(&message, data, sOverlayMessageSize);
	THROW_IF(message.mProtocolId != OVERLAY_PROTOCOL_ID, "protocol id is wrong");

	DEBUG_LOG(LL_DEBUG, LM_OVERLAY, "received overlay message: %s", ToString(message).c_str());
	mReceiveCounter += 1;
	mHandler.OnOverlayMessage(message);
}

void OverlayServer::ReportMetrics()
{
	LOG(LL_INFO, LM_OVERLAY, "received %d dmx overlays in the last 5 sec", mReceiveCounter);
	mReceiveCounter = 0;
}

}