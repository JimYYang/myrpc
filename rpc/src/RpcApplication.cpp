#include <rpc/RpcApplication.hpp>

void RpcApplication::init(int argc, char **argv)
{

}

RpcApplication &RpcApplication::getInstance()
{
    static RpcApplication app;
    return app;
}