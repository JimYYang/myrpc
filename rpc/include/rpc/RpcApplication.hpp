#pragma once

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