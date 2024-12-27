include(FetchContent)

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1)

FetchContent_MakeAvailable(catch2)

macro(config_load_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    add_test(NAME rpc.${TEST_NAME} COMMAND ${TEST_NAME})
    target_link_libraries(${TEST_NAME} PRIVATE rpc)
    target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)
endmacro()

config_load_test(test_config_load test_config_load.cpp)