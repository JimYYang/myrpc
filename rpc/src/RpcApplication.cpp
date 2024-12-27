#include <unistd.h>

#include <rpc/Logger.hpp>
#include <rpc/RpcApplication.hpp>
#include <spdlog/spdlog.h>
#include <string>

void showArgsHelp()
{
    spdlog::error("format: command -i <configFile>");
}

void RpcApplication::init(int argc, char **argv)
{
    if (argc < 2)
    {
        showArgsHelp();
        std::exit(EXIT_FAILURE);
    }

    Logger::init();

    int opt;
    std::string fileName;
    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            fileName = optarg;
            break;
        case ':':
            spdlog::error("Missing <configFile>");
            showArgsHelp();
            std::exit(EXIT_FAILURE);
        case '?':
            spdlog::error("Unknown option: {}", char(optopt));
            showArgsHelp();
            std::exit(EXIT_FAILURE);
            break;
        default:
            break;
        }
    }

    // 开始加载配置文件
    config_.loadConfigFile(fileName);
}

RpcApplication &RpcApplication::getInstance()
{
    static RpcApplication app;
    return app;
}