set(GTESTPATH ${THIRD_PATH}/gtest)
add_thirdparty_library(${GTESTPATH} gtest)

if(NOT EXISTS ${GTESTPATH}/lib/libgtest.a)

    externalproject_add(gtest_build
        DOWNLOAD_DIR ${GTESTPATH}
        URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
        URL_MD5 2d6ec8ccdf5c46b05ba54a9fd1d130d7
    #   SOURCE_DIR ${GTESTPATH}/build/
        CMAKE_ARGS
    #   CONFIGURE_COMMAND ./configure
        BUILD_COMMAND make
        INSTALL_COMMAND cp -rf include ${GTESTPATH}/ COMMAND mkdir -p ${GTESTPATH}/lib  COMMAND cp libgtest.a ${GTESTPATH}/lib/
        BUILD_IN_SOURCE 1
        INSTALL_DIR ${GTESTPATH}
    #   LOG_INSTALL 1
    #   LOG_BUILD 1
    #   LOG_CONFIGURE 1
    )

    add_custom_target(gtest_clean COMMAND rm -fr build
    COMMAND rm -fr include
    COMMAND rm -fr lib
    COMMAND rm -fr gtest-1.7.0
    COMMAND rm -fr gtest
    WORKING_DIRECTORY ${GTESTPATH}
    )

    add_dependencies(gtest gtest_build)
    add_dependencies(gtest_build gtest_clean)
endif()
