#include "Logger.h"

#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::vector<LogEntry> Logger::messages_;

void Logger::Initialise()
{
	const auto consoleSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
	consoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %v%$");

	std::vector<spdlog::sink_ptr> sinks{ consoleSink };
	const auto logger = std::make_shared<spdlog::logger>(DEFAULT_LOGGER_NAME, sinks.begin(), sinks.end());
	logger->set_level(spdlog::level::trace);
	logger->flush_on(spdlog::level::trace);
	spdlog::register_logger(logger);
}

void Logger::ShutDown()
{
	spdlog::shutdown();
}

void Logger::Trace(const std::string& message)
{
	spdlog::get(DEFAULT_LOGGER_NAME)->trace(message);
	const LogEntry log_entry{ spdlog::level::trace, message };
	messages_.push_back(log_entry);
}

void Logger::Debug(const std::string& message)
{
	spdlog::get(DEFAULT_LOGGER_NAME)->debug(message);
	const LogEntry log_entry{ spdlog::level::debug, message };
	messages_.push_back(log_entry);
}

void Logger::Warn(const std::string& message)
{
	spdlog::get(DEFAULT_LOGGER_NAME)->warn(message);
	const LogEntry log_entry{ spdlog::level::warn, message };
	messages_.push_back(log_entry);
}

void Logger::Fatal(const std::string& message)
{
	spdlog::get(DEFAULT_LOGGER_NAME)->critical(message + "\n Error in file: {} on line: {}", __FILE__, __LINE__);
	const LogEntry log_entry{ spdlog::level::critical, message };
	messages_.push_back(log_entry);
	__debugbreak();
}

void Logger::Log(const std::string& message)
{
	spdlog::get(DEFAULT_LOGGER_NAME)->info(message);
	const LogEntry log_entry{ spdlog::level::info, message };
	messages_.push_back(log_entry);
}

void Logger::Err(const std::string& message)
{
	spdlog::get(DEFAULT_LOGGER_NAME)->error(message);
	const LogEntry log_entry{ spdlog::level::err, message };
	messages_.push_back(log_entry);
}