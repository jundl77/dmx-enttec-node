#pragma once

#include <core/types.h>
#include <core/event_loop.h>

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>
#include <optional>

namespace DmxEnttecNode {

class Config
{
public:
	static std::optional<Config> FromFile(const std::string& filename);

	int mLogLevel;
	RunHot mRunHot { RunHot::No };
	int mCoreAffinity; // core number on which to run, only takes effect if mRunHot is RunHot::Yes
	std::string mAppName;
	uint32_t mControlListenPort;

	uint16_t mDmxUniverse;
	uint16_t mDmxAddressSpaceStart;
	uint16_t mDmxAddressSpaceRange;
};

}