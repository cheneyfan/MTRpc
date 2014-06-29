set(CMAKE_CXX_COMPILER "g++")
cmake_minimum_required(VERSION 2.6)

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SRC "*.[ch]pp")
get_filename_component(libname ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

message("Add ${libname} to build")
add_library(${libname} STATIC ${SRC} ${HEADERS})
set_target_properties(${libname} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-undefined")
