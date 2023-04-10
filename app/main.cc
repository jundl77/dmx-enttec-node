#include "app.h"

#include <config/config.h>
#include <core/logger.h>
#include <core/event_loop.h>
#include <core/clock.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

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
	SetGlobalLogLevel(LL_INFO);

	std::optional<Config> config = LoadConfig(argc, argv);
	if (!config)
	{
		LOG(LL_ERROR, LM_MAIN, "error loading config file, stopping");
		return 1;
	}

	config->mAppName = "dmx_enttec_node";
	config->mDeviceName = "PC";

	EventLoop loop;
	App app = App(*config, loop);
	app.Start();

	loop.Run(RunHot::Yes);
	return 0;
}