#include "clock.h"

#ifdef WIN32
#include <windows.h>
#endif

namespace DmxEnttecNode::tsc_impl {


inline uint64_t rdtscp()
{
#ifndef WIN32
	uint64_t rax, rcx, rdx;
	__asm__ __volatile__("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(rcx));
	return (rdx << 32) + rax;
#else
	// TODO: revise, this probably does not work
	return GetTickCount64();
#endif
}

inline void cpuid()
{
	// TODO: revise, this probably does not work

#ifndef WIN32
	uint64_t rax, rbx, rcx, rdx;
	__asm__ __volatile__("cpuid" : "=a"(rax), "=b"(rbx), "=d"(rdx), "=c"(rcx));
#endif
}

inline uint64_t rdtscp(int& chip, int& core)
{
#ifndef WIN32
	uint64_t rax, rcx, rdx;
	__asm__ __volatile__("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(rcx));
	chip = static_cast<int>((rcx & 0xFFF000) >> 12);
	core = static_cast<int>(rcx & 0xFFF);
	return (rdx << 32) + rax;
#else
	// TODO: revise, this probably does not work

	return GetTickCount64();
#endif
}

}