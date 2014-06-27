project(MIO CXX)
cmake_minimum_required(VERSION 2.8)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -msse -msse2 -msse3 -msse4.1 -g3 -fPIC -Wall -Wextra -Wno-unused-variable -Wno-unused-local-typedefs -Wno-unused-parameter -rdynamic -fpermissive -std=gnu++11 ")

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ")



set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out")


if(CMAKE_BUILD_RELEASE)
    set(CMAKE_BUILD_TYPE relwithdebinfo)
    set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()

# add thirdparty to globle includes
include(thirdparty/CMakeLists.txt)



add_subdirectory(libjson)
add_subdirectory(mio2)
add_subdirectory(test)
add_subdirectory(proto)

if(CMAKE_BUILD_TEST)
    # add test
    enable_testing()
    add_subdirectory(test)
endif()

