file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp) #头文件也可以不加
add_library(rpc STATIC ${srcs})
target_include_directories(rpc PUBLIC include ${CMAKE_SOURCE_DIR}/third_party)
find_package(spdlog REQUIRED)
target_link_libraries(rpc PRIVATE spdlog::spdlog_header_only)
# target_link_libraries(rpc PUBLIC spdlog::spdlog) spdlog：编译为动态库，然后链接的使用方式

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()