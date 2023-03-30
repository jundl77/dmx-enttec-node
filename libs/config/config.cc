#include "config.h"
#include <core/logger.h>
#include <core/njson.h>
#include <core/throw_if.h>
#include <fstream>

namespace DmxHueNode {

static const LogModule LM_CONFIG {"CONFIG"};

std::optional<Config> Config::FromFile(const std::string& filePath)
{
	LOG(INFO, LM_CONFIG, "loading config from file: " << filePath)

	std::ifstream ifs(filePath);
	if (!ifs.good())
	{
		LOG(ERROR, LM_CONFIG, "unable to find config file: " << filePath)
		return std::nullopt;
	}

	njson json = njson::parse(ifs);
	Config config {};

	LOG(INFO, LM_CONFIG, "loading config..")

	config.mControlListenPort = json["control_listen_port"];
	LOG(INFO, LM_CONFIG, "control_listen_port: " << config.mControlListenPort)

	config.mDataFolder = json["data_folder"];
	LOG(INFO, LM_CONFIG, "data_folder: " << config.mDataFolder)

	config.mDmxUniverse = json["dmx_universe"];
	THROW_IF(config.mDmxUniverse < 0, "dmx universe cannot be negative")
	LOG(INFO, LM_CONFIG, "dmx_universe: " << config.mDmxUniverse)

	config.mDmxAddressSpaceStart = json["dmx_address_space_start"];
	THROW_IF(config.mDmxAddressSpaceStart < 0, "dmx address space cannot be negative")
	THROW_IF(config.mDmxAddressSpaceStart > 512, "dmx address space cannot start above 512")
	LOG(INFO, LM_CONFIG, "dmx_address_space_start: " << config.mDmxAddressSpaceStart)

	config.mDmxAddressSpaceRange = json["dmx_address_space_range"];
	THROW_IF(config.mDmxAddressSpaceStart + config.mDmxAddressSpaceRange < 0, "dmx address space cannot end below 0")
	THROW_IF(config.mDmxAddressSpaceStart + config.mDmxAddressSpaceRange > 512, "dmx address space cannot end after 512")
	LOG(INFO, LM_CONFIG, "dmx_address_space_range: " << config.mDmxAddressSpaceStart)

	LOG(INFO, LM_CONFIG, "done loading config.")

	return config;
}

}