file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ./*.cpp ../include/*.cc)
add_executable(provider ${srcs})
message("provider ${srcs}")
target_link_libraries(provider rpc)
target_include_directories(provider PRIVATE ../include)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
target_link_libraries(provider ${Protobuf_LIBRARIES})
