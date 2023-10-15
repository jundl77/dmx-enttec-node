#pragma once

#include <config/config.h>
#include <core/event_loop.h>
#include <engine/engine.h>

#include <string>
#include <memory>

namespace DmxEnttecNode {

class AudioSender
{
public:
	AudioSender(const Config&, EventLoop& loop);
	~AudioSender();

	void Start();

private:
	const Config& mConfig;
	EventLoop& mEventLoop;
	Engine mEngine;
};

}