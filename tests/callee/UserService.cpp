#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <string>
#include <user.pb.h>
#include <spdlog/spdlog.h>

#include <rpc/RpcApplication.hpp>
#include <rpc/RpcProvider.hpp>

/**
 * @brief UserService原来是一个本地服务，提供了进程内的本地方法: Login和GetFriendLists
 * 
 */
class UserService : public fixbug::UserServiceRpc // 使用在rpc服务发布端（rpc服务提供者）
{
public:
    bool Login(std::string name, std::string pwd)
    {
        spdlog::info("Doing local service: Login.");
        spdlog::info("name: {}, pwd: {}.", pwd, name);
        return false;
    }

    /**
     * @brief 重写基类UserServiceRpc的虚函数，下面这些方法都是框架直接调用的
     * 1. caller ==> Login(LoginRequest) ==> muduo ==> callee
     * 2. callee ==> Login(LoginRequest) ==> 交到下面重写的Login方法
     * 
     */
     void Login(::google::protobuf::RpcController *controller,
                    const ::fixbug::LoginRequest *request,
                    ::fixbug::LoginResponse *response,
                    ::google::protobuf::Closure *done)
    {
        // 框架给业务上报了请求参数LoginRequest, 应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool loginResult = Login(name, pwd);

        // 把响应写入
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_sucess(loginResult);

        // 执行回调操作 执行响应对象数据的序列化和网络发送（均有框架完成）
        done->Run();
    }

};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作 
    RpcApplication::init(argc, argv);

    RpcProvider provider;
    provider.notifyService(new UserService{});
    provider.run();


    return 0;
}