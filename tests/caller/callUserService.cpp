#include <iostream>
#include <rpc/RpcApplication.hpp>
#include <user.pb.h>

int main(int argc, char **argv)
{
    // 程序启动后，先调用框架的初始化函数（只需要初始化一次）
    RpcApplication::init(argc, argv);



}