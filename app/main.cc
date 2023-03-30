#include "app.h"

#include <config/config.h>
#include <core/logger.h>
#include <core/tsc_clock.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>

static const LogModule LM_MAIN {"MAIN"};

std::optional<DmxHueNode::Config> LoadConfig(int argc, char *argv[])
{
	std::string filePath = "dmx_hue_node.json";
	if (argc == 2)
	{
		filePath = argv[1];
	}
	else if (argc > 2)
	{
		LOG(ERROR, LM_MAIN, "unable to parse arguments, usage: " << argv[0] << " [config_file]")
		//return std::nullopt;
	}

	return DmxHueNode::Config::FromFile(filePath);
}

int main(int argc, char *argv[])
{
	LOG(INFO, LM_MAIN, "==================================================")
	LOG(INFO, LM_MAIN, "Starting dmx-hue-node server")
	LOG(INFO, LM_MAIN, "==================================================")
	LOG(INFO, LM_MAIN, "")
	LOG(INFO, LM_MAIN, "")

	std::locale::global(std::locale("C"));
	std::cout.imbue(std::locale());
	std::cout << std::boolalpha;
	std::cin.imbue(std::locale());
	std::cerr.imbue(std::locale());

	std::signal(SIGINT, [](int signal) { std::exit(1); });

	DmxHueNode::TSCClock::Initialise();
	SetGlobalLogLevel(INFO);

	std::optional<DmxHueNode::Config> config = LoadConfig(argc, argv);
	if (!config)
	{
		LOG(ERROR, LM_MAIN, "error loading config file, stopping")
		return 1;
	}

	config->mAppName = "dmx_hue_node";
	config->mDeviceName = "PC";

	kj::AsyncIoContext asyncio = kj::setupAsyncIo();
	auto& waitScope = asyncio.waitScope;

	DmxHueNode::App app = DmxHueNode::App(*config, asyncio);
	auto serverPromise = app.Start();

	serverPromise.wait(waitScope);

	return 0;
}