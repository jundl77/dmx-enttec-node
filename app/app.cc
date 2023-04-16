#include "app.h"
#include <core/logger.h>

#include <string>
#include <iostream>

namespace DmxEnttecNode {

static const LogModule LM_APP {"APP"};

App::App(const Config& config, EventLoop& loop)
	: mConfig(config)
	, mEventLoop(loop)
	, mEngine(mConfig, mEventLoop)
{
}

App::~App()
{
}

void App::Start()
{
	LOG(LL_INFO, LM_APP, "starting %s", mConfig.mAppName.c_str());
	mEngine.Start();
}

}
