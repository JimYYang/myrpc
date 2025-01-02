# file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ./*.cpp ../include/user.pb.cc)
set(srcsUser callUserService.cpp ../include/user.pb.cc)
add_executable(consumerUser ${srcsUser})
target_link_libraries(consumerUser rpc)
target_include_directories(consumerUser PRIVATE ../include)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(consumerUser ${Protobuf_LIBRARIES})

# # 打印 Protobuf_INCLUDE_DIRS
# message(STATUS "Protobuf include directories: ${Protobuf_INCLUDE_DIRS}")

# # 打印 Protobuf_LIBRARIES
# message(STATUS "Protobuf libraries: ${Protobuf_LIBRARIES}")

# file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ./*.cpp ../include/user.pb.cc)
set(srcsFriend callFriendService.cpp ../include/friend.pb.cc)
add_executable(consumerFriend ${srcsFriend})
target_link_libraries(consumerFriend rpc)
target_include_directories(consumerFriend PRIVATE ../include)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(consumerFriend ${Protobuf_LIBRARIES})