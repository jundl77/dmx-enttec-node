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
{
}

Server::~Server()
{
}

void Server::Start()
{
	mArtnetServer.StartListening();
}

void Server::OnDmxMessage(const DmxFrame& frame)
{
	LOG(LL_INFO, LM_SERVER, "received dmx frame");
}

}