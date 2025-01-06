#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <functional>
#include <google/protobuf/descriptor.h>
#include <memory>
#include <netinet/in.h>
#include <rpc/RpcApplication.hpp>
#include <rpc/RpcChannel.hpp>
#include <rpc/RpcHeader.pb.h>
#include <rpc/ZkClient.hpp>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>

/**
 * @brief 所有通过stud代理对象调用的rpc方法，都走到这里，统一做rpc方法调用request的数据序列化和网络发送
 * headerSize(4 bytes) + (serviceName + methodName + argsSize) + args
 * @param method
 * @param controller
 * @param request
 * @param response
 * @param done
 */
void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                            google::protobuf::Message *response, google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string serviceName = sd->name();
    std::string methodName = method->name();

    // 获取参数的序列化字符串长度 argsSize
    uint32_t argsSize = 0;
    std::string argsStr;
    if (request->SerializeToString(&argsStr))
    {
        argsSize = argsStr.size();
    }
    else
    {
        spdlog::error("serialize request error!");
        controller->SetFailed("serialize request error!");
        return;
    }

    // 定义rpc的请求header
    rpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(serviceName);
    rpcHeader.set_method_name(methodName);
    rpcHeader.set_args_size(argsSize);

    uint32_t headerSize = 0;
    std::string rpcHeaderStr;
    if (rpcHeader.SerializeToString(&rpcHeaderStr))
    {
        headerSize = rpcHeaderStr.size();
    }
    else
    {
        spdlog::error("serialize rpc header error!");
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    spdlog::debug("consumer: headerSize: {}.", headerSize);
    spdlog::debug("consumer: rpcHeaderStr: {}", rpcHeaderStr);
    spdlog::debug("consumer: servieName: {}", serviceName);
    spdlog::debug("consumer: methodName: {}", methodName);
    spdlog::debug("consumer: argsSize: {}", argsSize);
    spdlog::debug("consumer: argsStr: {}", argsStr);

    // 组织待发送的rpc请求的字符串
    std::string sendRpcStr;
    sendRpcStr.insert(0, std::string((char *)&headerSize, 4));
    sendRpcStr += rpcHeaderStr;
    sendRpcStr += argsStr;

    // 使用tcp编程，完成rpc方法的远程调用
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientFd)
    {
        spdlog::error("create socket error! errno: {}, message: {}", errno, strerror(errno));
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno: %d, message: %s", errno, strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    std::unique_ptr<int, std::function<void(int *)>> clientFdPtr(new int(clientFd),
                                                                 [](int *fd)
                                                                 {
                                                                     if (fd && *fd != -1)
                                                                     {
                                                                         spdlog::info("Closing clientfd: {}", *fd);
                                                                         close(*fd);
                                                                         delete fd;
                                                                     }
                                                                 });

    // 读取配置文件的rpcserver信息
    // auto config = RpcApplication::getConfig();
    // auto oip = config.getConfig("rpc.ip");
    // std::string ip;
    // if (oip)
    // {
    //     ip = *oip;
    // }
    // else
    // {
    //     spdlog::error("rpc ip is EMPTY!");
    //     controller->SetFailed("rpc ip is EMPTY!");
    //     return;
    // }

    // auto oport = config.getConfig("rpc.port");
    // uint16_t port;
    // if (oport)
    // {
    //     port = std::stoi(*oport);
    // }
    // else
    // {
    //     spdlog::error("rpc port is EMPTY!");
    //     controller->SetFailed("rpc ip is EMPTY!");
    //     return;
    // }
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    std::string method_path = "/" + serviceName + "/" + methodName;
    // 127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); 

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (-1 == connect(clientFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))
    {
        spdlog::error("connect error! errno: {}, message: {}", errno, strerror(errno));
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno: %d, message: %s", errno, strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    // 发送rpc请求
    if (-1 == send(clientFd, sendRpcStr.c_str(), sendRpcStr.size(), 0))
    {
        spdlog::error("send error! errno: {}, message: {}", errno, strerror(errno));
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno: %d, message: %s", errno, strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    // 接收rpc请求的响应值
    char recvBuf[1024] = {0};
    int32_t recvSize = 0;
    if (-1 == (recvSize = recv(clientFd, recvBuf, 1024, 0)))
    {
        spdlog::error("recv error! errno: {}, message: {}", errno, strerror(errno));
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno: %d, message: %s", errno, strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化rpc调用的response
    // std::string responseStr(recvBuf, 0, recvSize);
    if (!response->ParseFromArray(recvBuf, recvSize))
    {
        spdlog::error("parse error! responseStr: {}.", recvBuf);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! responseStr: %s", recvBuf);
        controller->SetFailed(errtxt);
        return;
    }
}