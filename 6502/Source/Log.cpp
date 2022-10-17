#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

Log::Logger Log::s_Logger;
Log::Logger Log::s_CpuLogger;

void Log::Init()
{
	if (s_Logger != NULL)
		return;

	s_Logger = spdlog::stderr_color_mt("INFO");
	s_Logger->set_pattern("[%^%T]: %v%$");
	s_Logger->set_level(spdlog::level::info);

	s_CpuLogger = spdlog::stderr_color_mt("6502");
	s_CpuLogger->set_pattern("%v%$");
	s_CpuLogger->set_level(spdlog::level::trace);
}

void Log::Shutdown()
{
	s_Logger.reset();
	s_CpuLogger.reset();
}
