#define CATCH_CONFIG_MAIN // 定义主函数
// #include <catch2/catch.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rpc/RpcConfig.hpp>
#include <fstream>
#include <string>

// 测试用的配置文件路径
const std::string TEST_CONFIG_FILE = "test_config.json";

// 测试前准备：创建一个临时的 JSON 配置文件
void createTestConfigFile() {
    std::ofstream file(TEST_CONFIG_FILE);
    file << R"({
        "rpc": {
            "ip": "127.0.0.1",
            "port": 8000
        },
        "zookeeper": {
            "ip": "127.0.0.1",
            "port": 5000
        }
    })";
    file.close();
}

// 测试后清理：删除临时配置文件
void removeTestConfigFile() {
    std::remove(TEST_CONFIG_FILE.c_str());
}

TEST_CASE("RpcConfig can load configuration file", "[RpcConfig]") {
    // 创建测试配置文件
    createTestConfigFile();

    RpcConfig config;

    SECTION("Valid configuration file") {
        REQUIRE_NOTHROW(config.loadConfigFile(TEST_CONFIG_FILE)); // 检查不会抛出异常

        // 验证读取的内容是否正确
        REQUIRE(config.getConfig("rpc.ip") == "127.0.0.1");
        REQUIRE(config.getConfig("rpc.port") == "8000");
        REQUIRE(config.getConfig("zookeeper.ip") == "127.0.0.1");
        REQUIRE(config.getConfig("zookeeper.port") == "5000");
    }

    // SECTION("Invalid configuration file") {
    //     // 删除配置文件后测试加载无效文件
    //     removeTestConfigFile();
    //     REQUIRE_THROWS_AS(config.loadConfigFile(TEST_CONFIG_FILE), std::runtime_error);
    // }
}