#pragma once

#include "usb_client_if.h"
#include "enttec/libenttec/ftd2xx/ftd2xx.h"
#include <core/types.h>
#include <core/event_loop.h>
#include <core/scoped_handler.h>
#include <cstdint>

namespace DmxEnttecNode {

class EnttecUSBClient : public IEnttecUSBClient
{
public:
	explicit EnttecUSBClient(EventLoop&);
	~EnttecUSBClient();

	void Start();
	void Send(const DmxFrame&);

private:
	void FlushFrame();
	void ReloadDriver();
	void ReportMetrics();

	EventLoop& mEventLoop;
	FT_STATUS mFtStatus;
	uint8_t mNumDevices = 0;
	uint16_t mDeviceConnected = 0;

	unsigned char mSerializedFrame[DmxFrameSize + 1];
	bool mIsNewFrame = false;
	int mSendCounter = 0;

	ScopedHandler mSenderHandle;
	ScopedHandler mMetricsHandle;
};

}