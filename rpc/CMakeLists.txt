set(CMAKE_CXX_COMPILER "g++")
cmake_minimum_required(VERSION 2.6)

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SRC "*.[ch]pp")
get_filename_component(libname ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

message("Add ${libname} to build")
add_library(${libname} OBJECT ${SRC} ${HEADERS})
add_library(${libname}_static STATIC $<TARGET_OBJECTS:${libname}>)


#INSTALL(TARGETS ${libname}_static DESTINATION "./lib/")



SET(RPC_HEADES rpc_channel.h
rpc_client.h
rpc_controller.h
rpc_server.h
)
INSTALL(FILES ${RPC_HEADES} DESTINATION "./include/rpc/")
