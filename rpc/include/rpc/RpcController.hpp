#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <string>

class RpcController : public google::protobuf::RpcController
{
public:
    RpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string &reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);

private:
    bool failed_; // rpc方法执行过程中的状态
    std::string errText_; // rpc方法执行过程中的错误信息
};