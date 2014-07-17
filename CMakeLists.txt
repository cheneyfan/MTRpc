project(MTRpc_GIT CXX)
cmake_minimum_required(VERSION 2.8.9)


option(CMAKE_BUILD_RELEASE "build release version" OFF)
option(CMAKE_BUILD_TEST "build test " ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -g3 -fPIC ")


set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ")


if(CMAKE_BUILD_RELEASE)
    set(CMAKE_BUILD_TYPE relwithdebinfo)
    set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()

#for 64 bit system
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wall -Wextra -Wno-unused-variable -Wno-unused-local-typedefs -Wno-unused-parameter")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -msse4.1 ")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -msse4.1 ")
#for 32 bit system,how sb it is!
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wall -Wextra")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/out32")
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
add_subdirectory(thread)


#build all lib to mtrpc
add_library(mtrpc STATIC
            $<TARGET_OBJECTS:rpc>
            $<TARGET_OBJECTS:mio>
            $<TARGET_OBJECTS:proto>
            $<TARGET_OBJECTS:thread>
            $<TARGET_OBJECTS:log>
            $<TARGET_OBJECTS:common>
            $<TARGET_OBJECTS:inspect>
            $<TARGET_OBJECTS:json>
    )

INSTALL(TARGETS mtrpc DESTINATION "./lib/")


#### for test

if(CMAKE_BUILD_TEST)
    # add test
    enable_testing()
    add_subdirectory(test)
endif()


MESSAGE("Configure Done: BULD_TYPE:${CMAKE_BUILD_TYPE} FLAGS:${CMAKE_CXX_FLAGS_ALL}")


