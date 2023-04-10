#include "usb_client.h"
#include "libenttec/usb_pro.h"

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxEnttecNode {

EnttecUSBClient::~EnttecUSBClient()
{

}

void EnttecUSBClient::Start()
{
	FT_STATUS ftStatus;
	WORD wVID = 0x0403;
	WORD wPID = 0x6001;
	uint8_t Num_Devices =0;
	uint16_t device_connected =0;
	int i=0;
	int device_num=0;
	BOOL res = 0;

	printf("\nEnttec Pro - C - Windows - FTDI Test\n");
	printf("\nLooking for a PRO's connected to PC ... ");

	// Good idea to reload device drivers: takes a few secs
	/*
	printf ("\nReloading devices for use with drivers ");
	ftStatus = FT_Reload(wVID,wPID);
	Sleep(3500);
	if(ftStatus != FT_OK)
	{
		printf("\nReloading Driver FAILED");
	}
	else
		printf("\nReloading Driver D2XX PASSED");
	*/
	// Just to make sure the Device is correct
	printf("\n Press Enter to Intialize Device :");
	Num_Devices = FTDI_ListDevices();
	// Number of Found Devices
	if (Num_Devices == 0)
	{
		printf("\n Looking for Devices  - 0 Found");
	}
	else
	{
		// If you want to open all; use for loop
		// we'll open the first one only
		for (i=0;i<Num_Devices;i++)
		{
			if (device_connected)
				break;
			device_num = i;
			device_connected = FTDI_OpenDevice(device_num);
		}

/* Use this if you want to receive dmx in a loop
 */
		if (device_connected)
		{
			unsigned char myDmxIn[513];

			// Looping to receiving DMX data
			printf("\n Press Enter to receive DMX data :");
			printf("\nSet the widget to send DMX only when signal change... ");
			unsigned char send_on_change_flag = 1;
			res = FTDI_SendData(RECEIVE_DMX_ON_CHANGE,&send_on_change_flag,0);
			if (res < 0)
			{
				printf("FAILED\n");
				FTDI_ClosePort();
				return;
			}
			for (int i = 0; i < 1000 ; i++)
			{
				memset(myDmxIn,0,513);
				res = FTDI_RxDMX(SET_DMX_RX_MODE, myDmxIn, 513);
				if (res < 0)
				{
					printf("FAILED\n");
					FTDI_ClosePort();
					break;
					return;
				}
				printf("Iteration: %d\n", i);
				printf("DMX Data from 0 to 10: ");
				for (int j = 0; j <= 8; j++){
					printf (" %d ",myDmxIn[j]);
				}
			}

		}

// Use this if you want to send dmx in a loop
/*
		if (device_connected)
		{
			unsigned char myDmx[530];
			// Looping to Send DMX data
			printf("\n Press Enter to Send DMX data :");
			getch();
			for (int i = 0; i < 10000 ; i++)
			{
				memset(myDmx,i,530);
				myDmx[0] = 0;
				res = FTDI_SendData(6, myDmx, 75);
				if (res < 0)
				{
					printf("FAILED\n");
					FTDI_ClosePort();
					break;
					return -1;
				}
				printf("Iteration: %d\n", i);
				printf("DMX Data from 0 to 10: ");
				for (int j = 0; j <= 8; j++)
					printf (" %d ",myDmx[j]);

			}
		}
*/
	}

	// Finish all done
	printf("\n Press Enter to Exit :");
}

}