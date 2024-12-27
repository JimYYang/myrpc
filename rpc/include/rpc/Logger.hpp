#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

class Logger
{
public:
    static void init(const std::string &logFile = "/home/jsy/learn/myrpc/log/default.log");

    static std::shared_ptr<spdlog::logger> getLogger();

private:
    inline static std::shared_ptr<spdlog::logger> logger_ = nullptr;

};