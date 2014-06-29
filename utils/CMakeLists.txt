set(CMAKE_CXX_COMPILER "g++")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SRC "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library(utils STATIC ${SRC} ${HEADERS})
#target_link_libraries(utils trmem asn1c++ qos_client)

#set_target_properties(utils PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-undefined")
