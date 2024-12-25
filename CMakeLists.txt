cmake_minimum_required(VERSION 3.22)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(myrpc LANGUAGES CXX)

add_subdirectory(rpc)
add_subdirectory(tests)
