#pragma once

#include <config/config.h>
#include <core/event_loop.h>
#include <artnet/artnet_handler.h>
#include <artnet/artnet_server.h>
#include <overlay_server/overlay_server.h>
#include <overlay_server/overlay_handler.h>
#include <enttec/usb_client.h>
#include <core/app.h>

#include <string>
#include <memory>

namespace DmxEnttecNode {

class AppReceiverNode : public App, private IArtnetHandler, private IOverlayHandler
{
public:
	AppReceiverNode(const Config&, EventLoop& loop);
	~AppReceiverNode() override;

	void Start() override;
	const Config& GetConfig() const override { return mConfig; };

private:
	// IArtnetHandler
	void OnDmxMessage(const DmxFrame&, int universe) override;

	// IArtnetHandler
	void OnOverlayMessage(const OverlayIdl::OverlayMessage&) override;

private:
	const Config& mConfig;
	EventLoop& mEventLoop;
	ArtnetServer mArtnetServer;
	EnttecUSBClient mUsbClient;
	OverlayServer mOverlayServer;

	ScopedHandler mCbHandle;
};

}