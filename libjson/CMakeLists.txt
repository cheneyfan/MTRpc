set(CMAKE_CXX_COMPILER "g++")
cmake_minimum_required(VERSION 2.6)

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SRC "*.[ch]pp")


add_library(json STATIC ${SRC} ${HEADERS})
#set_target_properties(utils PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-undefined")
