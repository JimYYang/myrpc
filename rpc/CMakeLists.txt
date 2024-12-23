file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(rpc STATIC ${srcs})
target_include_directories(rpc PUBLIC include)