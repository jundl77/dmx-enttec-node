#include "types.h"

namespace DmxHueNode
{

std::string ToString(RgbChannel channel)
{
	switch (channel)
	{
		case RED:
			return "RED";
		case GREEN:
			return "GREEN";
		case BLUE:
			return "BLUE";
		default:
			throw std::runtime_error("unknown cpp rgb channel");
	}
}

RgbChannel FromString(const std::string& channel)
{
	if (channel == "RED")
		return RED;
	if (channel == "GREEN")
		return GREEN;
	if (channel == "BLUE")
		return BLUE;

	throw std::runtime_error("unknown cpp rgb channel");
}

}