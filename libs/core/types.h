#pragma once

#include "strong_typedef.h"
#include <huestream/common/data/Color.h>
#include <utility>
#include <functional>
#include <boost/container_hash/hash.hpp>

namespace DmxHueNode {

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
using HueLightId = StrongTypedef<std::string, struct HueLightIdTag>;
using DmxFrame = uint8_t*;
using HueChannel = std::pair<HueLightId, RgbChannel>;

struct Color
{
	double mRed;
	double mGreen;
	double mBlue;
};

}

namespace std {

template<>
struct hash<DmxHueNode::HueChannel>
{
	size_t operator()(const DmxHueNode::HueChannel& c) const
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, c.first);
		boost::hash_combine(seed, c.second);
		return seed;
	}
};

}