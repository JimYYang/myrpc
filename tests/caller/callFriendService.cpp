#include <rpc/RpcApplication.hpp>
#include <rpc/RpcChannel.hpp>
#include <friend.pb.h>
#include <spdlog/spdlog.h>

int main(int argc, char **argv)
{
    // 程序启动后，先调用框架的初始化函数（只需要初始化一次）
    RpcApplication::init(argc, argv);

    fixbug::FriendServiceRpc_Stub stub(new RpcChannel{});
    // rpc方法的请求参数
    fixbug::GetFriendsListRequest request;
    request.set_userid(2025);
    // rpc方法的响应
    fixbug::GetFirendsListResponse response;
    // 发起rpc方法的调用，同步的rpc调用过程
    stub.GetFriendsList(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，读取调用的结果
    if (0 == response.result().errcode())
    {
        spdlog::info("rpc GetFriends response success.");
        for (int i = 0; i < response.friends_size(); i++)
        {
            spdlog::info("friend index: {}, name: {}.", i + 1, response.friends(i));
        }
    }
    else
    {
        spdlog::error("rpc GetFriends response error: {}", response.result().errmsg());
    }

    return 0;
}