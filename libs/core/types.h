#pragma once

#include "strong_typedef.h"
#include <utility>
#include <functional>
#include <string>
#include <cstdint>
#include <span>

namespace DmxEnttecNode {

constexpr uint32_t DmxFrameSize = 512;

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
using DmxFrame = std::span<uint8_t, DmxFrameSize>;

struct Color
{
	double mRed;
	double mGreen;
	double mBlue;
};

}