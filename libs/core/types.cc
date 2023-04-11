#include "types.h"
#include <stdexcept>

namespace DmxEnttecNode
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

std::string ToString(const DmxFrame& frame)
{
#ifdef NDEBUG
	// don't compile this, because it's an expensive string computation
	return "";
#else
	std::string s = "\n";
	int index = 0;
	for (int i =0; i < 10; i++)
	{
		for (int y =0; y < 55; y++)
		{
			index += 1;
			if (index < frame.size())
			{
				s += std::to_string(frame[index]) + " ";
			}
		}
		s += "\n";
	}
	return s;
#endif
}

}