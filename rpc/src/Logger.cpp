#include <filesystem>
#include <iostream>
#include <rpc/Logger.hpp>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>


void Logger::init(const std::string &logFile)
{
    if (!logger_)
    {
        logger_ = spdlog::basic_logger_mt("Logger", logFile);
        spdlog::set_default_logger(logger_);
        spdlog::flush_on(spdlog::level::info);
        logger_->info("Logger initialized with file: {}", logFile);
    }
}

std::shared_ptr<spdlog::logger> Logger::getLogger()
{
    if (!logger_)
    {
        init();
    }
    return logger_;
}