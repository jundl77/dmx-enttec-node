#include "app.h"

#include <config/config.h>
#include <core/logger.h>
#include <core/event_loop.h>
#include <core/clock.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

#ifdef WIN32
#include <Windows.h>
#endif

using namespace DmxEnttecNode;

static const LogModule LM_MAIN {"MAIN"};

std::optional<Config> LoadConfig(int argc, char *argv[])
{
	std::string filePath = "dmx_enttec_node.json";
	if (argc == 2)
	{
		filePath = argv[1];
	}
	else if (argc > 2)
	{
		LOG(LL_ERROR, LM_MAIN, "unable to parse arguments, usage: %s [config_file]",  argv[0]);
		return std::nullopt;
	}

	return Config::FromFile(filePath);
}

bool SetAffinity(int core)
{
#ifdef WIN32
	HANDLE process = GetCurrentProcess();
	DWORD_PTR processAffinityMask = 1 << core;

	BOOL success = SetProcessAffinityMask(process, processAffinityMask);
	if (success)
	{
		return true;
	}
	return false;
#endif
	return false;
}

int main(int argc, char *argv[])
{
	LOG(LL_INFO, LM_MAIN, "==================================================");
	LOG(LL_INFO, LM_MAIN, "Starting dmx-enttec-node server");
	LOG(LL_INFO, LM_MAIN, "==================================================");
	LOG(LL_INFO, LM_MAIN, "");
	LOG(LL_INFO, LM_MAIN, "");

	std::locale::global(std::locale("C"));
	std::cout.imbue(std::locale());
	std::cout << std::boolalpha;
	std::cin.imbue(std::locale());
	std::cerr.imbue(std::locale());

	std::signal(SIGINT, [](int signal) { std::exit(1); });

	Clock::Initialise();
	SetGlobalLogLevel(LL_DEBUG);

	std::optional<Config> config = LoadConfig(argc, argv);
	if (!config)
	{
		LOG(LL_ERROR, LM_MAIN, "error loading config file, stopping");
		return 1;
	}
	SetGlobalLogLevel(config->mLogLevel);
	LOG(LL_INFO, LM_MAIN, "setting log-level to: %s", LogLevelToString(config->mLogLevel).c_str());

	if (config->mRunHot == RunHot::Yes)
	{
		LOG(LL_INFO, LM_MAIN, "running hot");
		SetAffinity(config->mCoreAffinity);
		LOG(LL_INFO, LM_MAIN, "set core affinity to core: %d", config->mCoreAffinity);
	}

	config->mAppName = "dmx_enttec_node";
	config->mDeviceName = "PC";

	EventLoop loop;
	App app = App(*config, loop);
	app.Start();

	try
	{
		loop.Run(config->mRunHot);
	}
	catch (const std::exception& e)
	{
		LOG(LL_ERROR, LM_MAIN, "program terminated with uncaught exception: %s", e.what());
		return 1;
	}
	return 0;
}