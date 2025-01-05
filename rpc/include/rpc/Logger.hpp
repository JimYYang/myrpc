#include <memory>
#include <spdlog/logger.h>
#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

class Logger
{
public:
    // 初始化日志器
    static void init(const std::string &logFileDir = "/home/jsy/learn/myrpc/log/");

    // 获取日志器
    static std::shared_ptr<spdlog::logger> getLogger();

private:
    inline static std::shared_ptr<spdlog::logger> logger_ = nullptr;                  // 日志器
    inline static std::shared_ptr<spdlog::details::thread_pool> thread_pool_ = nullptr; // 异步线程池
};