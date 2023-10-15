#pragma once

#include <config/config.h>
#include <core/event_loop.h>
#include <soundio/soundio.h>

#include <string>
#include <memory>

namespace DmxEnttecNode {

class AudioSender
{
public:
	AudioSender(const Config&, EventLoop& loop);
	~AudioSender();

	void Start();
	void SendAudioBytes(const char* data, size_t size);

private:
	const Config& mConfig;
	EventLoop& mEventLoop;

	SoundIo* mSoundIo;
	SoundIoDevice* mInputDevice;
	SoundIoInStream* mInStream;
};

}