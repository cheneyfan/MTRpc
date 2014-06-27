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
        add_library(${libname} STATIC IMPORTED)
        add_custom_target("${libname}_include" SOURCES ${HEADERS_H} ${HEADERS_HPP})
        set_property(TARGET ${libname} PROPERTY
                            IMPORTED_LOCATION ${DIR}/lib/lib${libname}.a)
        MESSAGE("add thirdparty lib ${libname}")
    endforeach()

endmacro(add_thirdparty_library)


## build every exe
macro(add_exe_item exefile)
    get_filename_component(TARGET ${exefile} NAME_WE)
    message("add exe item:${TARGET}")

    add_executable(${TARGET} ${exefile})
    target_link_libraries(${TARGET} ${ARGN})
endmacro(add_exe_item)



## set tcmaclloc
set(TCMACLLOCPATH ${THIRD_PATH}/gperf)
add_thirdparty_library(${TCMACLLOCPATH} profiler tcmalloc)


## set gtest
set(GTESTPATH ${THIRD_PATH}/gtest)
add_thirdparty_library(${GTESTPATH} gtest)

## set gmock
set(GMOCKPATH ${THIRD_PATH}/gmock)
add_thirdparty_library(${GMOCKPATH} gmock)


## set proto
set(PROTOPATH ${THIRD_PATH}/protobuf)
add_thirdparty_library(${PROTOPATH} protobuf)
