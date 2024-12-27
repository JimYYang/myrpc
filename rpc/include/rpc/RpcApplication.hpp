#pragma once

#include <rpc/RpcConfig.hpp>

// 框架的基础类，负责框架的一些初始化操作
class RpcApplication
{
public:
    static void init(int argc, char **argv);
    static RpcApplication &getInstance();

private:
    RpcApplication() {}
    RpcApplication(const RpcApplication &) = delete;
    RpcApplication(RpcApplication &&) = delete;

    inline static RpcConfig config_{}; // 使用C++17 inline在类内初始化静态成员变量
};