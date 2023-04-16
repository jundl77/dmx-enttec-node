#pragma once

#include <core/types.h>
#include <core/event_loop.h>
#include <artnet/artnet_server.h>
#include <enttec/usb_client.h>
#include <overlay_server/overlay_handler.h>
#include <overlay_server/overlay_server.h>

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxEnttecNode {

class Engine : private IArtnetHandler, private IOverlayHandler
{
public:
	Engine(const Config&, EventLoop& loop);
	~Engine();

	void Start();

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