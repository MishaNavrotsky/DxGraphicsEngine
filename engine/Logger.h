#pragma once

#include <memory>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "third_party/spdlog/sinks/basic_file_sink.h"
#include "third_party/spdlog/sinks/stdout_color_sinks.h"
#include "third_party/spdlog/spdlog.h"

class Logger {
public:
    static void Initialize() {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Engine.log", true));


        logSinks[0]->set_pattern("[%T] [%n] [%^%l%$] [%s:%# %!] %v");
        logSinks[1]->set_pattern("[%T] [%n] [%l] [%s:%# %!] %v");

        engineLogger = std::make_shared<spdlog::logger>("ENGINE", begin(logSinks), end(logSinks));
        spdlog::register_logger(engineLogger);
        engineLogger->set_level(spdlog::level::trace);
        engineLogger->flush_on(spdlog::level::trace);
    }

    inline static std::shared_ptr<spdlog::logger> &GetEngineLogger() { return engineLogger; }

private:
    inline static std::shared_ptr<spdlog::logger> engineLogger;
};


#define ENG_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(Logger::GetEngineLogger(), __VA_ARGS__)
#define ENG_LOG_INFO(...)     SPDLOG_LOGGER_INFO(Logger::GetEngineLogger(), __VA_ARGS__)
#define ENG_LOG_WARN(...)     SPDLOG_LOGGER_WARN(Logger::GetEngineLogger(), __VA_ARGS__)
#define ENG_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(Logger::GetEngineLogger(), __VA_ARGS__)
#define ENG_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Logger::GetEngineLogger(), __VA_ARGS__)
