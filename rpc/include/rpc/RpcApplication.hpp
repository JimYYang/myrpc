#pragma once

// 框架的基础类，负责框架的一些初始化操作
class RpcApplication
{
public:
    static void init(int argc, char **argv);
    static RpcApplication &getInstance();

private:
    RpcApplication(){}
    RpcApplication(const RpcApplication &) = delete;
    RpcApplication(RpcApplication &&) = delete;
};