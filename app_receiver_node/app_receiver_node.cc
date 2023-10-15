#include "app_receiver_node.h"
#include <core/logger.h>

#include <string>
#include <iostream>

namespace DmxEnttecNode {

static const LogModule LM_APP {"RECEIVER_APP"};

AppReceiverNode::AppReceiverNode(const Config& config, EventLoop& loop)
	: mConfig(config)
	, mEventLoop(loop)
	, mArtnetServer(mConfig, mEventLoop, *this)
	, mUsbClient(mEventLoop)
	, mOverlayServer(mConfig, mEventLoop, *this)
{
}

AppReceiverNode::~AppReceiverNode()
{
}

void AppReceiverNode::Start()
{
	LOG(LL_INFO, LM_APP, "starting %s", mConfig.mAppName.c_str());
	mArtnetServer.StartListening();
	mOverlayServer.Start();
	mUsbClient.Start();
}

void AppReceiverNode::OnDmxMessage(const DmxFrame& frame, int universe)
{
	DEBUG_LOG(LL_DEBUG, LM_APP, "received dmx frame on universe %d", universe);
	if (universe != mConfig.mDmxUniverse)
	{
		DEBUG_LOG(LL_DEBUG, LM_APP, "received dmx data for wrong universe, expected: %d, got %d",
				  mConfig.mDmxUniverse, universe);
		return;
	}
	mUsbClient.UpdateFrame(frame);
}

void AppReceiverNode::OnOverlayMessage(const OverlayIdl::OverlayMessage& msg)
{
	DEBUG_LOG(LL_DEBUG, LM_APP, "received overlay data on universe %d", msg.mUniverse);
	if (msg.mUniverse != mConfig.mDmxUniverse)
	{
		DEBUG_LOG(LL_DEBUG, LM_APP, "received overlay data for wrong universe, expected: %d, got %d",
				  mConfig.mDmxUniverse, msg.mUniverse);
		return;
	}
	mUsbClient.ApplyOverlay(msg, true);
}

}
