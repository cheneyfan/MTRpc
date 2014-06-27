set(CMAKE_CXX_COMPILER "g++")

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")



file(GLOB_RECURSE SRC "*.cpp")

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE HEADERS2 "*.hpp")
add_library(mio2 STATIC ${SRC} ${HEADERS} ${HEADERS2})

#cpplintcheck(mio)

#install(TARGETS aio DESTINATION ${CMAKE_INSTALL_PREFIX})
