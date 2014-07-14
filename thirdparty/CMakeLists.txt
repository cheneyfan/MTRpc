
cmake_minimum_required(VERSION 2.8)

set(THIRD_PATH ${CMAKE_SOURCE_DIR}/thirdparty)

#add third party
macro(add_thirdparty_library DIR)

    MESSAGE("include: ${DIR}/include")

    INCLUDE_DIRECTORIES(${DIR}/include)

    
    #file(GLOB_RECURSE HEADERS_H   "${DIR}/include/*.h")
    #file(GLOB_RECURSE HEADERS_HPP "${DIR}/include/*.hpp")

    foreach(header ${HEADERS_H})
        #set_source_files_properties(${header} PROPERTY HEADER_FILE_ONLY TRUE)
    endforeach()

    foreach(libname ${ARGN})


        #add_custom_target("${libname}_include" SOURCES ${HEADERS_H} ${HEADERS_HPP})

        add_library(${libname} STATIC IMPORTED)

        set_property(TARGET ${libname} PROPERTY
                            IMPORTED_LOCATION ${DIR}/lib/lib${libname}.a)
        MESSAGE("add thirdparty lib ${libname}")
    endforeach()

endmacro(add_thirdparty_library)





include(ExternalProject)

add_subdirectory(protobuf)
add_subdirectory(gtest)

