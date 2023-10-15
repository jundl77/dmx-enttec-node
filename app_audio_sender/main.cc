#include "app_audio_sender.h"
#include <core/app.h>

using namespace DmxEnttecNode;

int main(int argc, char *argv[])
{
	std::optional<AppContext> context = SetupApp(argc, argv, "audio_sender");
	if (!context)
	{
		return 1;
	}
	AppAudioSender app = AppAudioSender(context->mConfig, context->mEventLoop);
	RunApp(context->mEventLoop, app);
}