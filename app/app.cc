#include "app.h"
#include <core/logger.h>

#include <string>
#include <memory>
#include <iostream>

#define STREAMING_MODE_DTLS_STRING "dtls"

namespace DmxHueNode {

static const LogModule LM_APP {"APP"};
constexpr static auto PERSISTENCE_ENCRYPTION_KEY = "encryption_key";

App::App(const Config& config, kj::AsyncIoContext& asio)
	: mConfig(config)
	, mAsio(asio)
{
}

App::~App()
{
	mHueStream->ShutDown();
}

kj::Promise<void> App::Start()
{
	LOG(INFO, LM_APP, "starting " << mConfig.mAppName)

	InitializeHueStream();
	ConnectToBridge();

	mLightsSync = std::make_unique<LightsSync>(mConfig, mAsio, mHueStream);
	return mLightsSync->Start();
}

void App::ConnectToBridge()
{
	LOG(INFO, LM_APP, "connecting to bridge ")

	mHueStream->ConnectBridge();
	CheckConnection();

	LOG(INFO, LM_APP, "connected to bridge successfully")
}

void App::CheckConnection() const
{
	while (!this->mHueStream->IsBridgeStreaming())
	{
		auto bridge = this->mHueStream->GetLoadedBridge();

		if (bridge->GetStatus() == huestream::BRIDGE_INVALID_GROUP_SELECTED)
		{
			LOG(ERROR, LM_APP, "No valid bridge group selected")
		}
		else
		{
			LOG(ERROR, LM_APP, "No streamable bridge configured: " << bridge->GetStatusTag())
			this->mHueStream->ConnectBridge();
		}
	}
}

void App::InitializeHueStream()
{
	auto config = std::make_shared<huestream::Config>(mConfig.mAppName, mConfig.mDeviceName,
													  huestream::PersistenceEncryptionKey(PERSISTENCE_ENCRYPTION_KEY));
	config->GetAppSettings()->SetUseRenderThread(false);
	LOG(INFO, LM_APP, "initializing with streaming mode: " << STREAMING_MODE_DTLS_STRING)

	config->SetStreamingMode(huestream::STREAMING_MODE_DTLS);

	mHueStream = std::make_shared<LightSyncHueStream>(config);
	mHueStream->GetStream()->SetLightSyncStream(mHueStream);

	// Register feedback callback
	mHueStream->RegisterFeedbackCallback([](const huestream::FeedbackMessage &message)
	{
		LOG(DEBUG, LM_APP, "[" << message.GetId() << "] " << message.GetTag())

		if (message.GetId() == huestream::FeedbackMessage::ID_DONE_COMPLETED)
		{
			LOG(INFO, LM_APP, "bridge-ip: " << message.GetBridge()->GetIpAddress())
			LOG(INFO, LM_APP, "bridge-username: " << message.GetBridge()->GetUser())
			LOG(INFO, LM_APP, "bridge-clientkey: " << message.GetBridge()->GetClientKey())
		}

		if (message.GetMessageType() == huestream::FeedbackMessage::FEEDBACK_TYPE_USER)
		{
			LOG(INFO, LM_APP, message.GetUserMessage())
		}
	});
}

}
