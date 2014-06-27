set(CMAKE_CXX_COMPILER "g++")



file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SRC "*.cc")


add_library(proto-fs STATIC ${SRC} ${HEADERS})

