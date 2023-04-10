#pragma once

#include <cstdint>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>

namespace DmxEnttecNode {

class IEnttecUSBClient
{
	virtual ~IEnttecUSBClient() = default;
};

}