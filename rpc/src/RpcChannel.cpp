#include <cstdlib>
#include <functional>
#include <memory>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <rpc/RpcHeader.pb.h>
#include <rpc/RpcApplication.hpp>
#include <google/protobuf/descriptor.h>
#include <rpc/RpcChannel.hpp>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

/**
 * @brief 所有通过stud代理对象调用的rpc方法，都走到这里，统一做rpc方法调用request的数据序列化和网络发送
 * headerSize(4 bytes) + (serviceName + methodName + argsSize) + args 
 * @param method 
 * @param controller 
 * @param request 
 * @param response 
 * @param done 
 */
void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf::RpcController* controller, 
                        const google::protobuf::Message* request,
                        google::protobuf::Message* response, 
                        google::protobuf::Closure* done)
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
        return;
    }

    spdlog::info("-------------------");
    spdlog::debug("headerSize: {}.", headerSize);
    spdlog::debug("rpcHeaderStr: {}", rpcHeaderStr);
    spdlog::debug("servieName: {}", serviceName);
    spdlog::debug("methodName: {}", methodName);
    spdlog::debug("argsSize: {}", argsSize);
    spdlog::debug("argsStr: {}", argsStr);
    spdlog::info("-------------------");

    // 组织待发送的rpc请求的字符串
    std::string sendRpcStr;
    sendRpcStr.insert(0, std::string((char*)&headerSize, 4));
    sendRpcStr += rpcHeaderStr;
    sendRpcStr += argsStr;

    // 使用tcp编程，完成rpc方法的远程调用
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientFd)
    {
        spdlog::error("create socket error! errno: {}, message: {}", errno, strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    std::unique_ptr<int, std::function<void(int*)>> clientFdPtr(
        new int(clientFd),
        [](int *fd) {
            if (fd && *fd != -1)
            {
                spdlog::info("Closing clientfd: {}", *fd);
                close(*fd);
                delete fd;
            }   
        }
    );

    // 读取配置文件的rpcserver信息
    auto config = RpcApplication::getConfig();
    auto oip = config.getConfig("rpc.ip");
    std::string ip;
    if (oip)
    {
        ip = *oip;
    }
    else
    {
        spdlog::error("rpc ip is EMPTY!");
        std::exit(EXIT_FAILURE);
    }

    auto oport = config.getConfig("rpc.port");
    uint16_t port;
    if (oport)
    {
        port = std::stoi(*oport);
    }
    else
    {
        spdlog::error("rpc port is EMPTY!");
        std::exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (-1 == connect(clientFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        spdlog::error("connect error! errno: {}, message: {}", errno, strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    // 发送rpc请求
    if (-1 == send(clientFd, sendRpcStr.c_str(), sendRpcStr.size(), 0))
    {
        spdlog::error("send error! errno: {}, message: {}", errno, strerror(errno));
        return;
    }

    // 接收rpc请求的响应值
    char recvBuf[1024] = {0};
    int32_t recvSize = 0;
    if (-1 == (recvSize = recv(clientFd, recvBuf, 1024, 0)))
    {
        spdlog::error("recv error! errno: {}, message: {}", errno, strerror(errno));
        return;
    }

    // 反序列化rpc调用的response
    // std::string responseStr(recvBuf, 0, recvSize);
    if (!response->ParseFromArray(recvBuf, recvSize))
    {
        spdlog::error("parse error! responseStr: {}.", recvBuf);
        return;
    }

}