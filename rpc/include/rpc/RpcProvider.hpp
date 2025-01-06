#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <string>
#include <unordered_map>

// 框架提供的专门服务发布rpc服务的网络类对象
class RpcProvider
{
public:
    // 框架提供给外部使用的，可以发布rpc方法的函数接口
    void notifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程网络 调用服务
    void run();

private:
    muduo::net::EventLoop eventLoop_;
    // 这两个回调muduo自动回调
    // 新的socket连接回调
    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    // 已建立连接用户的读写事件回调
    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp timestamp);
    // Closure的回调操作，用于序列化rpc的response和网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response);

    // 服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *service_;                                                    // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> methodMap_; // 保存服务方法
    };

    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> serviceMap_;
};