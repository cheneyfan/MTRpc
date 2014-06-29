file(GLOB_RECURSE SRC "*.cpp")

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")



foreach(testfile ${SRC})
   add_exe_item(${testfile} mio  log common proto thread json gtest pthread rt)
endforeach()
