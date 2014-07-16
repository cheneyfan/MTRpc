
## build every exe
macro(add_exe_item exefile)
    get_filename_component(TARGET ${exefile} NAME_WE)
    message("add exe item:${TARGET}")

    add_executable(${TARGET} ${exefile})
    target_link_libraries(${TARGET} ${ARGN})
set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--no-undefined")
endmacro(add_exe_item)



file(GLOB_RECURSE SRC "*.cpp")

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")



foreach(testfile ${SRC})
   add_exe_item(${testfile}
          mtrpc 
          gtest
          protobuf
          pthread rt)
endforeach()
