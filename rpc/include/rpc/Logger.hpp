#include <memory>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

class Logger
{
public:
    static void init(const std::string &logFile = "/home/jsy/learn/myrpc/log/default.log");

    static std::shared_ptr<spdlog::logger> getLogger();

private:
    inline static std::shared_ptr<spdlog::logger> logger_ = nullptr;
};