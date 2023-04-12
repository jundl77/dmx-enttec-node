#include "server.h"
#include <core/logger.h>
#include <iostream>
#include <string>

namespace DmxEnttecNode {

static const LogModule LM_SERVER {"SERVER"};

Server::Server(const Config& config, EventLoop& loop)
	: mConfig(config)
	, mEventLoop(loop)
	, mArtnetServer(mConfig, mEventLoop, *this)
	, mUsbClient(mEventLoop)
{
}

Server::~Server()
{
}

void Server::Start()
{
	mArtnetServer.StartListening();
	mUsbClient.Start();
}

void Server::OnDmxMessage(const DmxFrame& frame, int universe)
{
	DEBUG_LOG(LL_DEBUG, LM_SERVER, "received dmx frame on universe %d", universe);
	if (universe != mConfig.mDmxUniverse)
	{
		DEBUG_LOG(LL_DEBUG, LM_SERVER, "received dmx data for wrong universe, expected: %d, got %d",
			mConfig.mDmxUniverse, universe);
		return;
	}
	mUsbClient.Send(frame);
}

}