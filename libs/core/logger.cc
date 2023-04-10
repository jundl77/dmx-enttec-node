#include "logger.h"

namespace DmxEnttecNode {

static uint8_t GLOBAL_LOG_LEVEL = LL_INFO;

void SetGlobalLogLevel(int logLevel)
{
	GLOBAL_LOG_LEVEL = logLevel;
}

uint8_t GetGlobalLogLevel()
{
	return GLOBAL_LOG_LEVEL;
}

}