#pragma once

#include <config/config.h>
#include <core/event_loop.h>
#include <engine/engine.h>

#include <string>
#include <memory>

namespace DmxEnttecNode {

class AudioReceiver
{
public:
	AudioReceiver(const Config&, EventLoop& loop);
	~AudioReceiver();

	void Start();

private:
	const Config& mConfig;
	EventLoop& mEventLoop;
	Engine mEngine;
};

}