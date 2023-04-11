#include "usb_client.h"
#include "libenttec/usb_pro.h"
#include <core/logger.h>
#include <core/throw_if.h>
#include <string>

namespace DmxEnttecNode {

static const LogModule LM_USB_CLIENT {"USB_CLIENT"};
static const WORD sVID = 0x0403;
static const WORD sPID = 0x6001;
static constexpr uint8_t DMX_PACKET_START_CODE = 0;

EnttecUSBClient::~EnttecUSBClient()
{
	FTDI_ClosePort();
}

void EnttecUSBClient::Start()
{
	LOG(LL_INFO, LM_USB_CLIENT, "starting enttec USB client, looking for a PRO connected to PC ...");
	ReloadDriver();

	mNumDevices = FTDI_ListDevices();
	THROW_IF(mNumDevices != 1, "expected 1 enttec USB PRO device, found " + std::to_string(mNumDevices));
	LOG(LL_INFO, LM_USB_CLIENT, "found number of devices: %d", mNumDevices);

	mDeviceConnected = FTDI_OpenDevice(0); // device 0 is first device
	THROW_IF(!mDeviceConnected, "failed to open device for reading/writing");
	LOG(LL_INFO, LM_USB_CLIENT, "opened device successfully for reading/writing");
}

bool EnttecUSBClient::Send(const DmxFrame& dmxFrame)
{
	THROW_IF(!mDeviceConnected, "tried to send DMX to enttec USB PRO, but device is not connected");

	// we need to shift the whole frame up by one to set DMX_PACKET_START_CODE
	unsigned char serializedFrame[DmxFrameSize + 1];
	std::memset(serializedFrame, 0, DmxFrameSize + 1);
	std::memcpy(serializedFrame + 1, dmxFrame.data(), dmxFrame.size());
	serializedFrame[0] = DMX_PACKET_START_CODE;

	int res = FTDI_SendData(SET_DMX_TX_MODE, serializedFrame, sizeof(serializedFrame));
	if (res < 0)
	{
		LOG(LL_WARN, LM_USB_CLIENT, "failed to send DMX to enttec USB PRO device");
		FTDI_ClosePort();
		return false;
	}
	DEBUG_LOG(LL_DEBUG, LM_USB_CLIENT, "sent DMX frame to USB device successfully, frame: %s", ToStream(dmxFrame).c_str());
}

bool EnttecUSBClient::Receive(DmxFrame& outFrame)
{
	THROW_IF(!mDeviceConnected, "tried to send DMX to enttec USB PRO, but device is not connected");

	unsigned char send_on_change_flag = 1;
	int res = FTDI_SendData(RECEIVE_DMX_ON_CHANGE,&send_on_change_flag,0);
	if (res < 0)
	{
		LOG(LL_INFO, LM_USB_CLIENT, "failed to set RECEIVE_DMX_ON_CHANGE on device");
		FTDI_ClosePort();
		return false;
	}

	std::memset(outFrame.data(), 0, 513);
	res = FTDI_RxDMX(SET_DMX_RX_MODE, outFrame.data(), 513);
	if (res < 0)
	{
		LOG(LL_INFO, LM_USB_CLIENT, "failed to read DMX message from enttec USB PRO device");
		FTDI_ClosePort();
		return false;
	}

	return true;
}

void EnttecUSBClient::ReloadDriver()
{
	LOG(LL_INFO, LM_USB_CLIENT, "reloading devices for use with drivers (wait 3.5sec)");
	mFtStatus = FT_Reload(sVID, sPID);
	Sleep(3500);
	if (mFtStatus != FT_OK)
	{
		LOG(LL_INFO, LM_USB_CLIENT, "reloading D2XX driver failed");
	}
	else
	{
		LOG(LL_INFO, LM_USB_CLIENT, "reloading D2XX driver passed");
	}
}

}