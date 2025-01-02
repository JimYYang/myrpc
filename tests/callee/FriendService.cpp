#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <string>
#include <friend.pb.h>
#include <spdlog/spdlog.h>

#include <rpc/RpcApplication.hpp>
#include <rpc/RpcProvider.hpp>
#include <vector>

/**
 * @brief UserService原来是一个本地服务，提供了进程内的本地方法: Login和GetFriendLists
 * 
 */
class FriendService : public fixbug::FriendServiceRpc // 使用在rpc服务发布端（rpc服务提供者）
{
public:
    // 具体的业务代码
    std::vector<std::string> GetFriendsList(uint32_t userId)
    {
        spdlog::info("do GetFriendsList service. UserId: {}.", userId);
        return {"11", "22", "33"};
    }

    /**
     * @brief 重写基类UserServiceRpc的虚函数，下面这些方法都是框架直接调用的
     * 1. caller ==> Login(LoginRequest) ==> muduo ==> callee
     * 2. callee ==> Login(LoginRequest) ==> 交到下面重写的Login方法
     * 
     */
     void GetFriendsList(::google::protobuf::RpcController *controller,
                    const ::fixbug::GetFriendsListRequest *request,
                    ::fixbug::GetFirendsListResponse *response,
                    ::google::protobuf::Closure *done)
    {
        // 框架给业务上报了请求参数GetFriendsListRequest, 应用获取相应数据做本地业务
        uint32_t userId = request->userid();

        // 做本地业务
        auto res = GetFriendsList(userId);

        // 把响应写入
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        for (std::string &name : res)
        {
            std::string *cur = response->add_friends();
            *cur = name;
        }

        // 执行回调操作 执行响应对象数据的序列化和网络发送（均由框架完成）
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作 
    RpcApplication::init(argc, argv);

    RpcProvider provider;
    provider.notifyService(new FriendService{});
    provider.run();

    return 0;
}