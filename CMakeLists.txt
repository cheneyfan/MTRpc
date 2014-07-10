project(MTRpcBak CXX)
cmake_minimum_required(VERSION 2.8)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -g3 -fPIC ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -msse4.1 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wall -Wextra -Wno-unused-variable -Wno-unused-local-typedefs -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 ")

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ")


if(CMAKE_BUILD_RELEASE)
    set(CMAKE_BUILD_TYPE relwithdebinfo)
    set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()



set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out")


# add thirdparty to globle includes
include(thirdparty/CMakeLists.txt)


add_subdirectory(common)
add_subdirectory(inspect)
add_subdirectory(json)
add_subdirectory(log)
add_subdirectory(mio)
add_subdirectory(proto)
add_subdirectory(rpc)
add_subdirectory(smart_ptr)
add_subdirectory(test)
add_subdirectory(thread)


if(CMAKE_BUILD_TEST)
    # add test
    enable_testing()
    add_subdirectory(test)
endif()

MESSAGE("BULD_TYPE:${CMAKE_BUILD_TYPE} FLAGS:${CMAKE_CXX_FLAGS_ALL}")


set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out")

