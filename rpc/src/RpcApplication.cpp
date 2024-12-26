#include <unistd.h>

#include <rpc/RpcApplication.hpp>
#include <spdlog/spdlog.h>
#include <string>

void showArgsHelp()
{
    // spdlog::error("format: command -i <configFile>");
}

void RpcApplication::init(int argc, char **argv)
{
    if (argc < 2)
    {
        showArgsHelp();
        exit(EXIT_FAILURE);
    }

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
                exit(EXIT_FAILURE);
            case '?':
                spdlog::error("Unknown option: {}", char(optopt));
                break;
            default:
                break;
        }
    }

    // 开始加载配置文件
}

RpcApplication &RpcApplication::getInstance()
{
    static RpcApplication app;
    return app;
}