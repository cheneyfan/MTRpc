
cmake_minimum_required(VERSION 2.8)

set(THIRD_PATH ${CMAKE_SOURCE_DIR}/thirdparty)


#add third party
macro(add_thirdparty_library DIR)

    MESSAGE("include: ${DIR}/include")

    INCLUDE_DIRECTORIES(${DIR}/include)

    
    file(GLOB_RECURSE HEADERS_H   "${DIR}/include/*.h")
    file(GLOB_RECURSE HEADERS_HPP "${DIR}/include/*.hpp")

    foreach(header ${HEADERS_H})
        #set_source_files_properties(${header} PROPERTY HEADER_FILE_ONLY TRUE)
    endforeach()

    foreach(libname ${ARGN})


        add_custom_target("${libname}_include" SOURCES ${HEADERS_H} ${HEADERS_HPP})

        add_library(${libname} STATIC IMPORTED)

        set_property(TARGET ${libname} PROPERTY
                            IMPORTED_LOCATION ${DIR}/lib/lib${libname}.a)
        MESSAGE("add thirdparty lib ${libname}")
    endforeach()

endmacro(add_thirdparty_library)





include(ExternalProject)

set(PROTOPATH ${THIRD_PATH}/protobuf)
add_thirdparty_library(${PROTOPATH} protobuf)
if(NOT EXISTS ${PROTOPATH}/lib/libprotobuf.a)
    include(thirdparty/protobuf/CMakeLists.txt)
endif()


set(GTESTPATH ${THIRD_PATH}/gtest)
add_thirdparty_library(${GTESTPATH} gtest)
MESSAGE("$GTESTPATH")
if(NOT EXISTS ${GTESTPATH}/lib/libgtest.a)
   include(thirdparty/gtest/CMakeLists.txt)
endif()

if(TCMALLOC_ENABLE)
    set(TCMALLOCPATH ${THIRD_PATH}/gperf)
    add_thirdparty_library(${TCMALLOCPATH} tcmalloc)
    if(NOT EXISTS ${TCMALLOCPATH}/lib/tcmalloc.a)
        include(thirdparty/gperf/CMakeLists.txt)
    endif()
endif()
