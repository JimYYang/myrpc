#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 框架读取配置文件类
class RpcConfig
{
public:
    // 解析加载配置文件
    void loadConfigFile(const std::string &configFile);

    // 得到配置信息
    std::string getConfig(const std::string &key);

private:
    std::unordered_map<std::string, std::string> configMap_;
    // 递归函数：将嵌套的 JSON 转换为平铺的键值对
    void flattenJson(const json& j, const std::string& prefix);
};