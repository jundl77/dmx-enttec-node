#pragma once

#include "clock.h"
#include "strong_typedef.h"
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <chrono>

namespace DmxEnttecNode {

#define LL_DEBUG 0
#define LL_INFO  1
#define LL_WARN  2
#define LL_ERROR 3

using LogModule = StrongTypedef<std::string, struct LogModuleTag>;
using Timestamp = std::chrono::system_clock::time_point;

void SetGlobalLogLevel(int logLevel);
uint8_t GetGlobalLogLevel();

inline NanoPosixTime ToNanoPosixTime(const Timestamp& timestamp)
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch());
}

inline NanoPosixTime CurrentNanoPosixTime()
{
	return ToNanoPosixTime(std::chrono::system_clock::now());
}

inline decltype(auto) GetLocalTime()
{
	const int64_t kT_ns_in_s = 1000000000;
	const size_t max_size = 35;
	auto time = CurrentNanoPosixTime().count();
	std::string result(max_size, ' ');

	time_t secs = (time_t)(time / kT_ns_in_s);
	struct tm tm_;
	size_t offset;
	localtime_r(&secs, &tm_);
	offset = strftime(result.data(), max_size, "%Y-%m-%d %H:%M:%S", &tm_);
	offset += sprintf(result.data() + offset, ".%09llu", static_cast<unsigned long long>((time % kT_ns_in_s)));
	result.resize(offset);
	return result;
}

#define LOG(level, module, ...) \
	do \
	{ \
		if (level == LL_DEBUG && GetGlobalLogLevel() == LL_DEBUG) \
		{ \
			std::cout << GetLocalTime() << " [Debug ] [" << module << "] "; \
			std::printf(__VA_ARGS__); \
			std::cout << std::endl; \
		} \
		else if (level == LL_INFO && GetGlobalLogLevel() <= LL_INFO) \
		{ \
			std::cout << GetLocalTime() << " [Info ] [" << module << "] "; \
			std::printf(__VA_ARGS__); \
			std::cout << std::endl; \
		} \
		else if (level == LL_WARN && GetGlobalLogLevel() <= LL_WARN) \
		{ \
			std::cout << GetLocalTime() << " [Warn ] [" << module << "] "; \
			std::printf(__VA_ARGS__); \
			std::cout << std::endl; \
		} \
		else if (level == LL_ERROR && GetGlobalLogLevel() <= LL_ERROR) \
		{ \
			std::cout << GetLocalTime() << " [Error ] [" << module << "] "; \
			std::printf(__VA_ARGS__); \
			std::cout << std::endl; \
		} \
	} while(false)

#ifdef NDEBUG
#define DEBUG_LOG(level, module, ...) {}
#else
#define DEBUG_LOG(level, module, ...) { LOG(level, module, __VA_ARGS__) }
#endif

}