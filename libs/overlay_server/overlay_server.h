#pragma once

#include "overlay_handler.h"
#include <core/types.h>
#include <core/event_loop.h>
#include <artnet/artnet_server.h>
#include <enttec/usb_client.h>

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxEnttecNode {

class OverlayServer
{
public:
	OverlayServer(const Config&, EventLoop& loop, IOverlayHandler&);
	~OverlayServer();

	void Start();

private:
	void StartServer(int port);
	void PollSocket();
	void OnData(const char*, int len);
	void ReportMetrics();

private:
	const Config& mConfig;
	EventLoop& mEventLoop;
	IOverlayHandler& mHandler;

	WSADATA mWsaData;
	int mSocket;
	sockaddr_in mClientAddr;
	char mRecvBuffer[4096];
	bool mRunning = false;
	int mReceiveCounter = 0;

	ScopedHandler mListenSocketPollHandle;
	ScopedHandler mMetricsHandle;
};

}