#pragma once

#include <chrono>
#include <stdint.h>
#include <thread>
#include <stdexcept>

namespace DmxEnttecNode {

using namespace std::chrono_literals;

using NanoPosixTime = std::chrono::nanoseconds;

namespace tsc_impl
{

inline uint64_t rdtscp()
{
	uint64_t rax, rcx, rdx;
	__asm__ __volatile__("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(rcx));
	return (rdx << 32) + rax;
}

inline void cpuid()
{
	uint64_t rax, rbx, rcx, rdx;
	__asm__ __volatile__("cpuid" : "=a"(rax), "=b"(rbx), "=d"(rdx), "=c"(rcx));
}

inline uint64_t rdtscp(int& chip, int& core)
{
	uint64_t rax, rcx, rdx;
	__asm__ __volatile__("rdtscp" : "=a"(rax), "=d"(rdx), "=c"(rcx));
	chip = static_cast<int>((rcx & 0xFFF000) >> 12);
	core = static_cast<int>(rcx & 0xFFF);
	return (rdx << 32) + rax;
}

static double& TSC_GetFrequencyGHz()
{
	static double TSCFreqGHz = .0;
	return TSCFreqGHz;
}

inline uint64_t TSC_NowInCycles()
{
	return rdtscp();
}

inline std::chrono::nanoseconds TSC_FromCycles(uint64_t cycles)
{
	const double nanoseconds{static_cast<double>(cycles) / TSC_GetFrequencyGHz()};
	return std::chrono::nanoseconds(static_cast<uint64_t>(nanoseconds));
}

inline NanoPosixTime TSC_Now()
{
	return TSC_FromCycles(TSC_NowInCycles());
}

inline void TSC_Initialise()
{
	double& tscFreq = TSC_GetFrequencyGHz();
	if (tscFreq != .0)
	{
		return;
	}

	using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
			std::chrono::high_resolution_clock,
			std::chrono::steady_clock>;

	int chip, core, chip2, core2;

	auto start = Clock::now();

	cpuid();
	uint64_t rdtsc_start = rdtscp(chip, core);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	uint64_t rdtsc_end = rdtscp(chip2, core2);
	cpuid();

	auto end = Clock::now();

	if (core != core2 || chip != chip2)
		throw std::runtime_error("tsc_clock: process needs to be pinned to a specific core");

	auto duration_s = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	uint64_t cycles = rdtsc_end - rdtsc_start;

	tscFreq = static_cast<double>(cycles) / static_cast<double>(duration_s.count());
}

}

class Clock {
public:
	static void Initialise();

	static NanoPosixTime Now();
};

inline NanoPosixTime Clock::Now()
{
#ifndef NDEBUG
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch().count() * 1ns);
#else
	return tsc_impl::TSC_Now();
#endif
}

inline void Clock::Initialise()
{
#ifndef NDEBUG
	// skip
#else
	tsc_impl::TSC_Initialise();
#endif
}

}