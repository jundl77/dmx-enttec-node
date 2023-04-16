#include "config.h"
#include <core/logger.h>
#include <core/njson.h>
#include <core/throw_if.h>
#include <fstream>

namespace DmxEnttecNode {

namespace {
const LogModule LM_CONFIG {"CONFIG"};

int ParseLogLevel(const std::string& logLevel)
{
	if (logLevel == "debug")
		return LL_DEBUG;
	if (logLevel == "info")
		return LL_INFO;
	if (logLevel == "warn")
		return LL_WARN;
	if (logLevel == "error")
		return LL_ERROR;

	throw std::runtime_error("unknown log-level '" + logLevel + "', options are: debug|info|warn|error");
}

RunHot ParseRunHot(bool runHot)
{
	if (runHot)
		return RunHot::Yes;
	return RunHot::No;
}

}

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

	std::string logLevel = json["log_level"];
	config.mLogLevel = ParseLogLevel(logLevel);
	LOG(LL_INFO, LM_CONFIG, "log_level: %s", LogLevelToString(config.mLogLevel).c_str());

	config.mRunHot = ParseRunHot(json["run_hot"]);
	LOG(LL_INFO, LM_CONFIG, "run_hot: %d", config.mRunHot);

	config.mCoreAffinity = json["core_affinity"];
	LOG(LL_INFO, LM_CONFIG, "core_affinity: %d", config.mCoreAffinity);

	config.mOverlayListenPort = json["overlay_listen_port"];
	LOG(LL_INFO, LM_CONFIG, "overlay_listen_port: %d", config.mOverlayListenPort);

	config.mDmxUniverse = json["dmx_universe"];
	THROW_IF(config.mDmxUniverse < 0, "dmx universe cannot be negative");
	LOG(LL_INFO, LM_CONFIG, "dmx_universe: %d", config.mDmxUniverse);

	LOG(LL_INFO, LM_CONFIG, "done loading config.");

	return config;
}

}