#include <rpc/Logger.hpp>

// 根据当前日期生成日志文件名
std::string Logger::generateLogFileName(const std::string &logFileDir)
{
    // 获取当前时间
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    // 格式化日期并拼接文件名
    std::ostringstream oss;
    oss << logFileDir << "log_" << std::put_time(&tm, "%Y-%m-%d") << ".log";
    return oss.str();
}

void Logger::init(const std::string &logFileDir, size_t maxFileSize, size_t maxFiles)
{
    if (!logger_)
    {
        // 创建异步线程池
        size_t queue_size = 8192; // 队列大小
        thread_pool_ = std::make_shared<spdlog::details::thread_pool>(queue_size, 1);

        // 自动生成按日期命名的日志文件名
        std::string logFileName = generateLogFileName(logFileDir);

        // 创建旋转日志 sink（限制大小 + 文件编号）
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFileName, maxFileSize, maxFiles);

        // 创建异步日志器
        logger_ = std::make_shared<spdlog::async_logger>("Logger", spdlog::sinks_init_list{rotating_sink}, // 单一 sink
                                                         thread_pool_,
                                                         spdlog::async_overflow_policy::block); // 阻塞式溢出策略

        // 设置默认日志器
        spdlog::set_default_logger(logger_);

        // 设置日志刷新策略
        spdlog::flush_on(spdlog::level::info);

        // 设置日志级别
        spdlog::set_level(spdlog::level::debug);

        // 初始化日志器完成的消息
        logger_->info("Logger initialized. Logging to: {}", logFileDir);
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