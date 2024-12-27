#include <fstream>
#include <iostream>
#include <rpc/RpcConfig.hpp>
#include <spdlog/spdlog.h>

// 解析加载配置文件
void RpcConfig::loadConfigFile(const std::string &configFile)
{
    std::ifstream file(configFile);
    try
    {
        std::ifstream file(configFile);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open configuration file: " + configFile);
        }

        json config;
        file >> config;

        // 将 JSON 转换为平铺的键值对
        flattenJson(config, "");

        spdlog::info("Configuration loaded successfully.");
        spdlog::info("{}", configMap_["rpc.ip"]);
        spdlog::info("{}", configMap_["rpc.port"]);
        spdlog::info("{}", configMap_["zookeeper.ip"]);
        spdlog::info("{}", configMap_["zookeeper.port"]);
        
    }
    catch (const json::exception &e)
    {
        spdlog::error("JSON parsing error: {}.", e.what());
        std::exit(EXIT_FAILURE); // 直接退出程序
    }
    catch (const std::runtime_error &e)
    {
        spdlog::error("JRuntime error: {}.", e.what());
        std::exit(EXIT_FAILURE); // 直接退出程序
    }
    catch (...)
    {
        spdlog::error("An unknown error occurred.");
        std::exit(EXIT_FAILURE); // 直接退出程序
    }
}

// 得到配置信息
std::string RpcConfig::getConfig(const std::string &key) {
    auto it = configMap_.find(key);
    if (it != configMap_.end()) {
        return it->second;
    }
    return ""; // 返回空字符串表示未找到
}

// 递归函数：将嵌套的 JSON 转换为平铺的键值对
void RpcConfig::flattenJson(const json &j, const std::string &prefix)
{
    for (auto it = j.begin(); it != j.end(); ++it)
    {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        if (it->is_object())
        {
            // 如果值是对象，递归处理
            flattenJson(*it, key);
        }
        else
        {
            // 如果值是字符串，直接存储值；否则转换为字符串
            if (it->is_string())
            {
                configMap_[key] = it->get<std::string>();
            }
            else
            {
                configMap_[key] = it->dump(); // 转换为字符串存储
            }
        }
    }
}