#pragma once

#include "usb_client_if.h"

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxEnttecNode {

class EnttecUSBClient : public IEnttecUSBClient
{
	EnttecUSBClient() = default;
	~EnttecUSBClient() = default;
};

}