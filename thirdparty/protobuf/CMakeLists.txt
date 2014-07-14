set(PROTOPATH ${THIRD_PATH}/protobuf)
add_thirdparty_library(${PROTOPATH} protobuf)

if(NOT EXISTS ${PROTOPATH}/lib/libprotobuf.a)
MESSAGE("build")
externalproject_add(protobuf_build
    DOWNLOAD_DIR ${PROTOPATH}
    URL https://protobuf.googlecode.com/files/protobuf-2.5.0.tar.gz
    URL_MD5 b751f772bdeb2812a2a8e7202bf1dae8
    SOURCE_DIR ${PROTOPATH}/build
    CONFIGURE_COMMAND ./configure --enable-shared=no --prefix=${PROTOPATH}
    BUILD_COMMAND make -j
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${PROTOPATH}
)

add_dependencies(protobuf protobuf_build)
endif()
