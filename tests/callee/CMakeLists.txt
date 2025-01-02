# file(GLOB_RECURSE srcsUser CONFIGURE_DEPENDS ./*.cpp ../include/*.cc)
set (srcsUser UserService.cpp ../include/user.pb.cc)
add_executable(providerUser ${srcsUser})
target_link_libraries(providerUser rpc)
target_include_directories(providerUser PRIVATE ../include)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(providerUser ${Protobuf_LIBRARIES})

set (srcsFriend FriendService.cpp ../include/friend.pb.cc)
add_executable(providerFriend ${srcsFriend})
target_link_libraries(providerFriend rpc)
target_include_directories(providerFriend PRIVATE ../include)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(providerFriend ${Protobuf_LIBRARIES})
