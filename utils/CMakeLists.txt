set(CMAKE_CXX_COMPILER "g++")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/public/aio")
include_directories("${CMAKE_SOURCE_DIR}/public/log")
include_directories("${CMAKE_SOURCE_DIR}/public/mio")
include_directories("${CMAKE_SOURCE_DIR}/thirdparty/trmem_api_asn/include")

file(GLOB_RECURSE SRC "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library(utils STATIC ${SRC} ${HEADERS})
#target_link_libraries(utils trmem asn1c++ qos_client)

#set_target_properties(utils PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-undefined")
