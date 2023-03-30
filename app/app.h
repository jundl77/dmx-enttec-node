#pragma once

#include <config/config.h>
#include <server/lights_sync.h>
#include <hue_client/lights_sync_stream.h>

#include <huestream/config/Config.h>
#include <huestream/HueStream.h>

#include <string>
#include <memory>

namespace DmxHueNode {

using huestream::HueStreamPtr;

class App
{
public:
	App(const Config&, kj::AsyncIoContext&);
	~App();

	kj::Promise<void> Start();

private:
	void InitializeHueStream();
	void ConnectToBridge();
	void CheckConnection() const;

private:
	const Config& mConfig;
	kj::AsyncIoContext& mAsio;

	std::unique_ptr<LightsSync> mLightsSync;
	std::shared_ptr<LightSyncHueStream> mHueStream;
};

}