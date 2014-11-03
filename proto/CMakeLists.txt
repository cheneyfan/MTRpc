include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")


file(GLOB PROTO_INPUT RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.proto")
#file(GLOB_RECURSE PROTO_INPUT "*.proto")

#find_program(PROTOCBIN protoc)
set(PROTOC_HOME "${CMAKE_SOURCE_DIR}/thirdparty/protobuf")
set(PROTOCMD "${PROTOC_HOME}/bin/protoc")
set(PROTOCMD  ${PROTOCMD} " --proto_path=. --proto_path=${PROTOC_HOME}/include ")

# add cc files at cmake time
set(PROTOCMDSHELL  ${PROTOCMD} " --cpp_out=. ")
set(PROTOCMDSHELL  "${PROTOCMDSHELL} ${PROTO_INPUT}")

STRING(REPLACE ";" " " PROTO_CMD_SHELL "${PROTOCMDSHELL}")


file(GLOB HEADERS RELATIVE  ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c[c|p]*")

file(GLOB PROTO_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.pb.h")
MESSAGE("PROTO:${PROTO_SOURCES}")
if(NOT PROTO_SOURCES)
EXECUTE_PROCESS(COMMAND sh -c ${PROTO_CMD_SHELL}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c[c|p]*")
endif()

#include
#set(PROTOCMD  ${PROTOCMD} " --proto_path=. --proto_path=${PROTOC_HOME}/include ")
#set(PROTOCMD  ${PROTOCMD} " --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ")
#set(PROTOCMD  "${PROTOCMD} ${PROTO_INPUT}")
#STRING(REPLACE ";" " " PROTO_CMD "${PROTOCMD}")


#foreach(PROTO_FILE ${PROTO_INPUT})
#    #get the name of the file without extension
#    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
#    set(PROTO_GEN_CC "${PROTO_NAME}.pb.cc")
#    #add the generated files
#    set(PROTO_GEN  ${PROTO_GEN} ${PROTO_GEN_CC})
#    set_source_files_properties("${PROTO_GEN_CC}" PROPERTIES GENERATED TRUE)
#endforeach(PROTO_FILE ${PROTO_INPUT})



#gen cc file at compile time
#add_custom_command(
#OUTPUT  ${PROTO_GEN}
#COMMAND sh -c ${PROTO_CMD};
#DEPENDS ${PROTOC} ${PROTO_FILE}
#WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)

add_custom_target("PROTO_FILES" SOURCES ${PROTO_INPUT})

get_filename_component(libname ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
message("Add ${libname} to build ${PROTO_GEN}")

message("Add ${libname} to build")
add_library(${libname} OBJECT ${SOURCES} ${HEADERS})
#add_library(${libname}_static STATIC $<TARGET_OBJECTS:${libname}>)

add_dependencies(${libname} protobuf)

#INSTALL(TARGETS ${libname}_static DESTINATION "./lib/")


file(GLOB_RECURSE RPC_HEADES "*.pb.h")
INSTALL(FILES ${RPC_HEADES} DESTINATION "./include/proto/")
