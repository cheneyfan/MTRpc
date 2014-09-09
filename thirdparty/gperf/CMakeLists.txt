MESSAGE("ADD externalproject tcmalloc:${TCMALLOCPATH}")

externalproject_add(tcmalloc_build
    PREFIX ${TCMALLOCPATH}
    DOWNLOAD_DIR ${TCMALLOCPATH}
    URL https://googledrive.com/host/0B6NtGsLhIcf7MWxMMF9JdTN3UVk/gperftools-2.2.1.tar.gz
    URL_MD5 47cbede9442764f9e8fbc403af6cacfd
    SOURCE_DIR ${TCMALLOCPATH}/build
    CONFIGURE_COMMAND ./configure --enable-shared=no --prefix=${TCMALLOCPATH}
    BINARY_DIR ${TCMALLOCPATH}/build
    BUILD_COMMAND make -j
    #BUILD_IN_SOURCE 1
    INSTALL_DIR ${TCMALLOCPATH}
)


add_custom_target(tcmalloc_clean COMMAND rm -fr build
    COMMAND rm -fr include
    COMMAND rm -fr lib
    COMMAND rm -fr gperftools-2.2.1
    COMMAND rm -fr bin
    COMMAND rm -fr share
    WORKING_DIRECTORY ${TCMALLOCPATH}
)

add_dependencies(tcmalloc tcmalloc_build)
#add_dependencies(tcmalloc_build tcmalloc_clean)
