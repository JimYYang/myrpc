#include <rpc/RpcApplication.hpp>
#include <rpc/RpcChannel.hpp>
#include <user.pb.h>
#include <spdlog/spdlog.h>

int main(int argc, char **argv)
{
    // 程序启动后，先调用框架的初始化函数（只需要初始化一次）
    RpcApplication::init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new RpcChannel{});
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应
    fixbug::LoginResponse response;
    // 发起rpc方法的调用，同步的rpc调用过程
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，读取调用的结果
    if (0 == response.result().errcode())
    {
        spdlog::info("rpc Login response success: {}", response.sucess());
    }
    else
    {
        spdlog::error("rpc Login response error: {}", response.result().errmsg());
    }

    fixbug::RegisterRequest req;
    req.set_id(2025);
    req.set_name("jim");
    req.set_pwd("2025");

    fixbug::RegisterResponse rsp;

    // 以同步的方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr, &req, &rsp, nullptr);

    // 一次rpc调用完成，读取调用的结果
    if (0 == rsp.result().errcode())
    {
        spdlog::info("rpc Register response success: {}", rsp.sucess());
    }
    else
    {
        spdlog::error("rpc Register response error: {}", rsp.result().errmsg());
    }
    
    return 0;
}