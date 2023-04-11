#pragma once

#include "usb_client_if.h"
#include "enttec/libenttec/ftd2xx/ftd2xx.h"
#include <core/types.h>
#include <cstdint>

namespace DmxEnttecNode {

class EnttecUSBClient : public IEnttecUSBClient
{
public:
	~EnttecUSBClient();

	void Start();
	bool Send(const DmxFrame&);

private:
	void ReloadDriver();
	bool Receive(DmxFrame& outFrame);

	FT_STATUS mFtStatus;
	uint8_t mNumDevices = 0;
	uint16_t mDeviceConnected =0;
};

}