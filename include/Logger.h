#pragma once
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

#define DEFAULT_LOGGER_NAME "caperlogger"

struct LogEntry
{
	spdlog::level::level_enum type; //log type
	std::string message;
};
class Logger
{
public:
	static void Initialise();
	static void ShutDown();

	static std::vector<LogEntry> messages_;

	static void Trace(const std::string& message);
	static void Debug(const std::string& message);
	static void Log(const std::string& message); // Info
	static void Warn(const std::string& message);
	static void Err(const std::string& message);
	static void Fatal(const std::string& message);
};
