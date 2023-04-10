#pragma once

#include <core/types.h>
#include <core/event_loop.h>
#include <artnet/artnet_server.h>

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxEnttecNode {

class Server : private IArtnetHandler
{
public:
	Server(const Config&, EventLoop& loop);
	~Server();

	void Start();

private:
	// IArtnetHandler
	void OnDmxMessage(const DmxFrame&) override;

private:
	const Config& mConfig;
	EventLoop& mEventLoop;
	ArtnetServer mArtnetServer;

	ScopedHandler mCbHandle;
};

}