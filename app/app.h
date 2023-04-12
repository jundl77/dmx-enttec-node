#pragma once

#include <config/config.h>
#include <core/event_loop.h>
#include <server/server.h>

#include <string>
#include <memory>

namespace DmxEnttecNode {

class App
{
public:
	App(const Config&, EventLoop& loop);
	~App();

	void Start();

private:
	const Config& mConfig;
	EventLoop& mEventLoop;
	Server mServer;
};

}