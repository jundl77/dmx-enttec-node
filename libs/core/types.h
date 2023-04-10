#pragma once

#include "strong_typedef.h"
#include <utility>
#include <functional>
#include <string>
#include <cstdint>

namespace DmxEnttecNode {

enum RgbChannel
{
	RED,
	GREEN,
	BLUE
};

std::string ToString(RgbChannel);
RgbChannel FromString(const std::string&);

using ClientId = StrongTypedef<uint64_t, struct ClientIdTag>;
using DmxChannel = StrongTypedef<uint16_t, struct DmxChannelTag>;
using DmxFrame = uint8_t*;

struct Color
{
	double mRed;
	double mGreen;
	double mBlue;
};

}