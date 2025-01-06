#include <ctime>
#include <iomanip>
#include <memory>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

class Logger
{
public:
    static void init(const std::string &logFileDir = "/home/jsy/learn/myrpc/log/",
                     size_t maxFileSize = 50 * 1024 * 1024, // 单文件大小限制
                     size_t maxFiles = 365);                // 最大保留文件数

    static std::shared_ptr<spdlog::logger> getLogger();

private:
    static std::string generateLogFileName(const std::string &logFileDir);

    inline static std::shared_ptr<spdlog::logger> logger_ = nullptr;                    // 日志器
    inline static std::shared_ptr<spdlog::details::thread_pool> thread_pool_ = nullptr; // 异步线程池
};