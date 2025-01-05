#include <rpc/Logger.hpp>

void Logger::init(const std::string &logFileDir)
{
    if (!logger_)
    {
        // 创建异步线程池
        size_t queue_size = 8192; // 队列大小
        thread_pool_ = std::make_shared<spdlog::details::thread_pool>(queue_size, 1);

        // 创建按日期分割的文件 sink
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            logFileDir + "log.log", 0, 0); // 每天 00:00 创建新文件

        // 创建异步日志器
        logger_ = std::make_shared<spdlog::async_logger>(
            "Logger",
            spdlog::sinks_init_list{daily_sink}, // 使用按日期分割的 sink
            thread_pool_,
            spdlog::async_overflow_policy::block);

        // 设置默认日志器
        spdlog::set_default_logger(logger_);

        // 设置日志刷新策略
        spdlog::flush_on(spdlog::level::info);

        // 设置日志输出级别
        spdlog::set_level(spdlog::level::debug);

        // 初始化完成日志消息
        logger_->info("Logger initialized with directory: {}", logFileDir);
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