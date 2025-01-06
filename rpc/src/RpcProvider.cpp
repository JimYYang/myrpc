#include <cstdint>
#include <cstdlib>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <rpc/RpcApplication.hpp>
#include <rpc/RpcHeader.pb.h>
#include <rpc/RpcProvider.hpp>
#include <rpc/ZkClient.hpp>
#include <spdlog/spdlog.h>
#include <string>

/**
 * @brief service_name ==> service描述
                        ==> service* 记录服务对象
                        method_name ==> method对象
 *
 * @param service
 */

// 框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::notifyService(google::protobuf::Service *service)
{
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *serviceDescPtr = service->GetDescriptor();
    // 获取服务的名字
    std::string serviceName = serviceDescPtr->name();
    // 获取服务对象servie的方法的数量
    int32_t methodCnt = serviceDescPtr->method_count();

    ServiceInfo serviceInfo;

    for (int i = 0; i < methodCnt; i++)
    {
        // 获取了服务对象指定下标的服务方法的描述（抽象描述）
        const auto *methodDescPtr = serviceDescPtr->method(i);
        serviceInfo.methodMap_.emplace(methodDescPtr->name(), methodDescPtr);
    }
    serviceInfo.service_ = service;
    serviceMap_.emplace(serviceName, serviceInfo);
}

// 启动rpc服务节点，开始提供rpc远程网络 调用服务
void RpcProvider::run()
{
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

    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&eventLoop_, address, "RpcProvider");
    // 绑定连接回调和消息读写回调 连接线程，IO线程
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(
        std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : serviceMap_) 
    {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.methodMap_)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    spdlog::info("RpcProvider start service at ip: {}, port: {}.", ip, port);

    server.start();
    eventLoop_.loop();
}

/**
 * @brief rpc请求是短连接请求
 *        服务端返回响应之后主动关闭连接
 *
 */

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client的连接断开了
        conn->shutdown();
    }
}

/**
 * @brief 已建立连接用户读写事件的回调
 *        如果远程有一个rpc服务的调用请求，那么onMessage方法就会相应
 *        在框架内部，RpcProvider和RpcConsumer要协商好之间通信的protobuf数据类型
 *             serviceName methodName args        定义proto的message类型，进行数据头的序列化和反序列化
               16UserServiceLoginzhang san123456  headerStr：serviceName methodName argsSize
               headerSize(4 bytes) + headerStr + argsStr
 *
 * @param conn
 * @param buf
 * @param timestamp
 */
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf,
                            muduo::Timestamp timestamp)
{
    // 网络上接收的远程rpc调用请求的字节流  Login  args
    std::string recvBuf = buf->retrieveAllAsString();

    // 从字节流中读取前4个字节的内容
    uint32_t headerSize = 0;
    recvBuf.copy((char *)&headerSize, 4, 0);

    // 根据headerSize读取数据头的原始字节流，反序列化数据，得到rpc请求的详细信息
    std::string rpcHeaderStr = recvBuf.substr(4, headerSize);
    rpc::RpcHeader rpcHeader;
    std::string serviceName;
    std::string methodName;
    uint32_t argsSize;
    if (rpcHeader.ParseFromString(rpcHeaderStr))
    {
        // 数据头反序列化成功
        serviceName = rpcHeader.service_name();
        methodName = rpcHeader.method_name();
        argsSize = rpcHeader.args_size();
    }
    else
    {
        spdlog::error("rpcHeaderStr: {}, parse error!", rpcHeaderStr);
        return;
    }

    // 获取rpc方法的字节流数据
    std::string argsStr = recvBuf.substr(4 + headerSize, argsSize);

    spdlog::debug("provider: headerSize: {}.", headerSize);
    spdlog::debug("provider: rpcHeaderStr: {}", rpcHeaderStr);
    spdlog::debug("provider: servieName: {}", serviceName);
    spdlog::debug("provider: methodName: {}", methodName);
    spdlog::debug("provider: argsSize: {}", argsSize);
    spdlog::debug("provider: argsStr: {}", argsStr);

    // 获取service对象和method对象
    auto it = serviceMap_.find(serviceName);
    if (it == serviceMap_.end())
    {
        spdlog::error("{} is not exist!", serviceName);
        return;
    }

    auto mit = it->second.methodMap_.find(methodName);
    if (mit == it->second.methodMap_.end())
    {
        spdlog::error("{}'s {} method is not exist!", serviceName, methodName);
        return;
    }

    google::protobuf::Service *service = it->second.service_;       // 获取service对象
    const google::protobuf::MethodDescriptor *method = mit->second; // 获取service的method方法

    // 生成rpc方法调用的请求request和相应response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(argsStr))
    {
        spdlog::error("request parse error, argsStr: {}.", argsStr);
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    // new UserService()->Login()(controller, request, response, done)

    // 给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure *done =
        google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(
            this, &RpcProvider::sendRpcResponse, conn, response);
    service->CallMethod(method, nullptr, request, response, done);
}

// Closure的回调操作，用于序列化rpc的response和网络发送
void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string responseStr;
    if (response->SerializeToString(&responseStr))
    {
        // 序列化成功后，通过网络把rpc方法执行的结果发送给rpc的调用方
        // 这个打印是看不出来的，因为protobuf序列化之后中间有\0，不能完整显示出来
        spdlog::info("send response: {}.", responseStr);
        conn->send(responseStr);
    }
    else
    {
        spdlog::error("serialize responseStr error!");
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpc provider主动断开连接
}