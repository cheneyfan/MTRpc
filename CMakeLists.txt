project(MTRpc_SVN CXX)
cmake_minimum_required(VERSION 2.8.9)


option(CMAKE_BUILD_RELEASE "build release version" OFF)
option(CMAKE_BUILD_TEST "build test " ON)
option(TCMALLOC_ENABLE "use tcmalloc " ON)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(PKGVERSION_FULL "${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_PATCH}")

#IF(EXISTS "${CMAKE_SOURCE_DIR}/.svn/")
   FIND_PACKAGE(Subversion)
   IF(SUBVERSION_FOUND)
     Subversion_WC_INFO(${CMAKE_SOURCE_DIR} MT)
     string(REPLACE  "\n" " \\n"  VERSION_REVISION ${MT_WC_INFO})
   ELSE()
     MESSAGE("No found svn package")
   ENDIF(SUBVERSION_FOUND)
#ELSE()
#   MESSAGE("No svn project")
#ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/.svn/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -g3 -fPIC -rdynamic ")

#for 64 bit system
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wall -Wextra -Wl,--eh-frame-hdr")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -msse4.1 ")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-unused-local-typedefs -Wno-unused-parameter")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ")

    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/mtprpc_64")
#for 32 bit system,how sb it is!
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686 ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wall -Wextra")
   
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std= ")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/mtrpc_32")
endif()

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ")


if(CMAKE_BUILD_RELEASE)
    set(CMAKE_BUILD_TYPE relwithdebinfo)
    set(CMAKE_CXX_FLAGS_ALL "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()





# add thirdparty to globle includes
include(thirdparty/CMakeLists.txt)


add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(json)
add_subdirectory(log)
add_subdirectory(mio)
add_subdirectory(proto)
add_subdirectory(rpc)
add_subdirectory(smart_ptr)
add_subdirectory(thread)


#build all lib to mtrpc
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
add_library(mtrpc STATIC
            $<TARGET_OBJECTS:rpc>
            $<TARGET_OBJECTS:mio>
            $<TARGET_OBJECTS:proto>
            $<TARGET_OBJECTS:http>
            $<TARGET_OBJECTS:thread>
            $<TARGET_OBJECTS:log>
            $<TARGET_OBJECTS:common>
            $<TARGET_OBJECTS:json>
    )
else()
    add_library(mtrpc SHARED
            $<TARGET_OBJECTS:rpc>
            $<TARGET_OBJECTS:mio>
            $<TARGET_OBJECTS:proto>
            $<TARGET_OBJECTS:http>
            $<TARGET_OBJECTS:thread>
            $<TARGET_OBJECTS:log>
            $<TARGET_OBJECTS:common>
            $<TARGET_OBJECTS:inspect>
            $<TARGET_OBJECTS:json>
    )
    target_link_libraries(mtrpc protobuf rt pthread)
    set_target_properties(mtrpc PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-undefined") 
endif()

INSTALL(TARGETS mtrpc DESTINATION "./lib/")


#### for test

if(CMAKE_BUILD_TEST)
    # add test
    enable_testing()
    add_subdirectory(test)
endif()

configure_file("${CMAKE_SOURCE_DIR}/version.h.in" "${CMAKE_SOURCE_DIR}/version.h")

MESSAGE("Configure Done: BULD_TYPE:${CMAKE_BUILD_TYPE} FLAGS:${CMAKE_CXX_FLAGS_ALL}")


