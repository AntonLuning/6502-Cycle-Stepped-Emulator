#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <string>
#include <sstream>
#include <iomanip>

class Log
{
public:
	using Logger = std::shared_ptr<spdlog::logger>;

	static void Init();
	static void Shutdown();

	inline static Logger& GetLogger() { return s_Logger; }
	inline static Logger& GetCpuLogger() { return s_CpuLogger; }

	template<typename T>
	static std::string WordToHexString(T value)
	{
		std::stringstream stream;
		stream << "$" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::uppercase << std::hex << (WORD)value;

		return stream.str();
	}

private:
	static Logger s_Logger;
	static Logger s_CpuLogger;
};

#define LOG_INFO(...)		::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)		::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)	::Log::GetLogger()->critical(__VA_ARGS__)

#define PRINT_CPU(...)		::Log::GetCpuLogger()->trace(__VA_ARGS__)