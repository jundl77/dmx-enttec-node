#include "config.h"
#include <core/logger.h>
#include <core/njson.h>
#include <core/throw_if.h>
#include <fstream>

namespace DmxEnttecNode {

static const LogModule LM_CONFIG {"CONFIG"};

std::optional<Config> Config::FromFile(const std::string& filePath)
{
	LOG(LL_INFO, LM_CONFIG, "loading config from file: %s", filePath.c_str());

	std::ifstream ifs(filePath);
	if (!ifs.good())
	{
		LOG(LL_ERROR, LM_CONFIG, "unable to find config file: %s", filePath.c_str());
		return std::nullopt;
	}

	njson json = njson::parse(ifs);
	Config config {};

	LOG(LL_INFO, LM_CONFIG, "loading config..");

	config.mControlListenPort = json["control_listen_port"];
	LOG(LL_INFO, LM_CONFIG, "control_listen_port: %d", config.mControlListenPort);

	config.mDmxUniverse = json["dmx_universe"];
	THROW_IF(config.mDmxUniverse < 0, "dmx universe cannot be negative");
	LOG(LL_INFO, LM_CONFIG, "dmx_universe: %d", config.mDmxUniverse);

	config.mDmxAddressSpaceStart = json["dmx_address_space_start"];
	THROW_IF(config.mDmxAddressSpaceStart < 0, "dmx address space cannot be negative");
	THROW_IF(config.mDmxAddressSpaceStart > 512, "dmx address space cannot start above 512");
	LOG(LL_INFO, LM_CONFIG, "dmx_address_space_start: %d", config.mDmxAddressSpaceStart);

	config.mDmxAddressSpaceRange = json["dmx_address_space_range"];
	THROW_IF(config.mDmxAddressSpaceStart + config.mDmxAddressSpaceRange < 0, "dmx address space cannot end below 0");
	THROW_IF(config.mDmxAddressSpaceStart + config.mDmxAddressSpaceRange > 512, "dmx address space cannot end after 512");
	LOG(LL_INFO, LM_CONFIG, "dmx_address_space_range: %d", config.mDmxAddressSpaceStart);

	LOG(LL_INFO, LM_CONFIG, "done loading config.");

	return config;
}

}