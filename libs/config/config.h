#pragma once

#include "types.h"

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxHueNode {

class Config
{
public:
	static std::optional<Config> FromFile(const std::string& filename);

	std::string mAppName;
	std::string mDeviceName;
	std::string mDataFolder;
	uint32_t mControlListenPort;

	uint16_t mDmxUniverse;
	uint16_t mDmxAddressSpaceStart;
	uint16_t mDmxAddressSpaceRange;
};

}