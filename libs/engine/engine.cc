#include "engine.h"
#include <core/logger.h>
#include <iostream>
#include <string>

namespace DmxEnttecNode {

static const LogModule LM_SERVER {"SERVER"};

Engine::Engine(const Config& config, EventLoop& loop)
	: mConfig(config)
	, mEventLoop(loop)
	, mArtnetServer(mConfig, mEventLoop, *this)
	, mUsbClient(mEventLoop)
	, mOverlayServer(mConfig, mEventLoop, *this)
{
}

Engine::~Engine()
{
}

void Engine::Start()
{
	mArtnetServer.StartListening();
	mOverlayServer.Start();
	mUsbClient.Start();
}

void Engine::OnDmxMessage(const DmxFrame& frame, int universe)
{
	DEBUG_LOG(LL_DEBUG, LM_SERVER, "received dmx frame on universe %d", universe);
	if (universe != mConfig.mDmxUniverse)
	{
		DEBUG_LOG(LL_DEBUG, LM_SERVER, "received dmx data for wrong universe, expected: %d, got %d",
			mConfig.mDmxUniverse, universe);
		return;
	}
	mUsbClient.UpdateFrame(frame);
}

void Engine::OnOverlayMessage(const OverlayIdl::OverlayMessage& msg)
{
	DEBUG_LOG(LL_DEBUG, LM_SERVER, "received overlay data on universe %d", msg.mUniverse);
	if (msg.mUniverse != mConfig.mDmxUniverse)
	{
		DEBUG_LOG(LL_DEBUG, LM_SERVER, "received overlay data for wrong universe, expected: %d, got %d",
				  mConfig.mDmxUniverse, msg.mUniverse);
		return;
	}
	mUsbClient.ApplyOverlay(msg);
}

}