#include "app_receiver_node.h"
#include <core/app.h>

using namespace DmxEnttecNode;

int main(int argc, char *argv[])
{
	std::optional<AppContext> context = SetupApp(argc, argv, "receiver_node");
	if (!context)
	{
		return 1;
	}
	AppReceiverNode app = AppReceiverNode(context->mConfig, context->mEventLoop);
	RunApp(context->mEventLoop, app);
}