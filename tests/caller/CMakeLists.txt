file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ./*.cpp ../include/user.pb.cc)
add_executable(consumer ${srcs})
target_link_libraries(consumer rpc)
target_include_directories(consumer PRIVATE ../include)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(consumer ${Protobuf_LIBRARIES})

# # 打印 Protobuf_INCLUDE_DIRS
# message(STATUS "Protobuf include directories: ${Protobuf_INCLUDE_DIRS}")

# # 打印 Protobuf_LIBRARIES
# message(STATUS "Protobuf libraries: ${Protobuf_LIBRARIES}")