#include "audio_sender.h"
#include "audio_utils.h"
#include <config/config.h>
#include <proto/soundio_proto.h>
#include <core/throw_if.h>
#include <core/logger.h>
#include <string>

namespace DmxEnttecNode {

namespace {

const LogModule LM_SENDER {"AUDIO_SENDER"};

AudioSender* sAudioSender {nullptr};
const double sMicrophoneLatencySec = 0.05;


void ReadCallback(SoundIoInStream* instream, int frameCountMin, int frameCountMax)
{
	SoundIoChannelArea *areas;
	int err;
	int writeFrames = frameCountMax;
	int framesLeft = writeFrames;
	int maxReadSize = writeFrames * instream->bytes_per_frame;

	static std::vector<char> framesBuffer;
	if (framesBuffer.capacity() < maxReadSize)
	{
		framesBuffer.reserve(maxReadSize * 16);
	}
	std::memset(framesBuffer.data(), 0, maxReadSize);
	int framesBufferIndex = 0;

	for (;;)
	{
		int frameCount = framesLeft;
		THROW_IF((err = soundio_instream_begin_read(instream, &areas, &frameCount)),
				 "begin read error: " + std::string(soundio_strerror(err)));

		if (!frameCount)
			break;

		if (!areas)
		{
			// Due to an overflow there is a hole. Fill the ring buffer with silence for the size of the hole.
			memset(framesBuffer.data() + framesBufferIndex, 0, frameCount * instream->bytes_per_frame);
			framesBufferIndex += frameCount * instream->bytes_per_frame;
			LOG(LL_ERROR, LM_SENDER, "Dropped %d frames due to internal overflow", frameCount);
		}
		else
		{
			for (int frame = 0; frame < frameCount; frame += 1)
			{
				for (int ch = 0; ch < instream->layout.channel_count; ch += 1)
				{
					memcpy(framesBuffer.data() + framesBufferIndex, areas[ch].ptr, instream->bytes_per_sample);
					areas[ch].ptr += areas[ch].step;
					framesBufferIndex += instream->bytes_per_sample;
				}
			}
		}

		THROW_IF((err = soundio_instream_end_read(instream)),
				 "end read error: " + std::string(soundio_strerror(err)));

		framesLeft -= frameCount;
		if (framesLeft <= 0)
			break;
	}

	THROW_IF(framesBufferIndex > maxReadSize, "copied more bytes than we predicted, something is wrong");
	sAudioSender->SendAudioBytes(framesBuffer.data(), maxReadSize);
}

}

AudioSender::AudioSender(const Config& config, EventLoop& loop)
	: mConfig(config)
	, mEventLoop(loop)
	, mUdpClient(LM_SENDER, loop)
{
	THROW_IF(sAudioSender != nullptr, "only one audio-sender instance can be active");
	sAudioSender = this;

	mSoundIo = CreateSoundIo(DEFAULT_SOUNDIO_BACKEND);
}

AudioSender::~AudioSender()
{
	soundio_instream_destroy(mInStream);
	soundio_device_unref(mInputDevice);
	soundio_destroy(mSoundIo);
}

void AudioSender::Start()
{
	mUdpClient.Connect(mConfig.mAudioServerIpv4, mConfig.mAudioServerPort);
	StartAudioRecorder();
}

void AudioSender::StartAudioRecorder()
{
	const bool isRawDevice = false; // a raw device directly interacts with the hardware
	mInputDevice = GetAudioInputDevice(mSoundIo, mConfig.mAudioDeviceId, isRawDevice);
	LOG(LL_INFO, LM_SENDER, "using audio input device: %s", mInputDevice->name);
	LogSupportedFormats(LM_SENDER, mInputDevice);
	LogSupportedSampleRates(LM_SENDER, mInputDevice);
	LOG(LL_INFO, LM_SENDER, "using format '%s' and sample rate '%d'", mConfig.mAudioFormat.c_str(), mConfig.mAudioSampleRate);

	mInStream = soundio_instream_create(mInputDevice);
	THROW_IF(!mInStream, "could not setup soundio audio input, out of memory");
	mInStream->format = SoundIoFormatFromString(mConfig.mAudioFormat);
	mInStream->sample_rate = mConfig.mAudioSampleRate;
	mInStream->software_latency = sMicrophoneLatencySec;
	mInStream->read_callback = ReadCallback;

	int err;
	THROW_IF((err = soundio_instream_open(mInStream)),
			 "unable to open input stream: " + std::string(soundio_strerror(err)));
	THROW_IF((err = soundio_instream_start(mInStream)),
			 "unable to start input device: " + std::string(soundio_strerror(err)));

	mEventLoop.AddTimer(1ms, [this]() { soundio_wait_events(mSoundIo); });
}

void AudioSender::SendAudioBytes(const char* data, size_t size)
{
	if (size > SOUNDIO_MAX_DATA_SIZE)
	{
		LOG(LL_INFO, LM_SENDER, "dropping data of size %d, because it is too big (max=%d)", size, SOUNDIO_MAX_DATA_SIZE);
		return;
	}

	static SoundIoIdl::SoundIoData soundioData {};
	soundioData.mSeqNum = ++mSeqNum;
	soundioData.mSize = size;
	std::memcpy(soundioData.mData, data, size);

	mUdpClient.SendData(&soundioData, sizeof(SoundIoIdl::SoundIoData));
	LOG(LL_DEBUG, LM_SENDER, "sent audio data, seqnum=%d, size=%d", soundioData.mSeqNum, sizeof(SoundIoIdl::SoundIoData));
}

}