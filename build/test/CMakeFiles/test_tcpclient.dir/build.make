# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/guofutan/workspace/MTRpc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/guofutan/workspace/MTRpc/build

# Include any dependencies generated for this target.
include test/CMakeFiles/test_tcpclient.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test_tcpclient.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test_tcpclient.dir/flags.make

test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o: test/CMakeFiles/test_tcpclient.dir/flags.make
test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o: ../test/test_tcpclient.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/guofutan/workspace/MTRpc/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o"
	cd /home/guofutan/workspace/MTRpc/build/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o -c /home/guofutan/workspace/MTRpc/test/test_tcpclient.cpp

test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.i"
	cd /home/guofutan/workspace/MTRpc/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/guofutan/workspace/MTRpc/test/test_tcpclient.cpp > CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.i

test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.s"
	cd /home/guofutan/workspace/MTRpc/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/guofutan/workspace/MTRpc/test/test_tcpclient.cpp -o CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.s

test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.requires:
.PHONY : test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.requires

test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.provides: test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test_tcpclient.dir/build.make test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.provides.build
.PHONY : test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.provides

test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.provides.build: test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o

# Object files for target test_tcpclient
test_tcpclient_OBJECTS = \
"CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o"

# External object files for target test_tcpclient
test_tcpclient_EXTERNAL_OBJECTS =

test/test_tcpclient: test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o
test/test_tcpclient: test/CMakeFiles/test_tcpclient.dir/build.make
test/test_tcpclient: mio/libmio.a
test/test_tcpclient: log/liblog.a
test/test_tcpclient: common/libcommon.a
test/test_tcpclient: proto/libproto.a
test/test_tcpclient: thread/libthread.a
test/test_tcpclient: json/libjson.a
test/test_tcpclient: ../thirdparty/gtest/lib/libgtest.a
test/test_tcpclient: test/CMakeFiles/test_tcpclient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_tcpclient"
	cd /home/guofutan/workspace/MTRpc/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_tcpclient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test_tcpclient.dir/build: test/test_tcpclient
.PHONY : test/CMakeFiles/test_tcpclient.dir/build

test/CMakeFiles/test_tcpclient.dir/requires: test/CMakeFiles/test_tcpclient.dir/test_tcpclient.cpp.o.requires
.PHONY : test/CMakeFiles/test_tcpclient.dir/requires

test/CMakeFiles/test_tcpclient.dir/clean:
	cd /home/guofutan/workspace/MTRpc/build/test && $(CMAKE_COMMAND) -P CMakeFiles/test_tcpclient.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test_tcpclient.dir/clean

test/CMakeFiles/test_tcpclient.dir/depend:
	cd /home/guofutan/workspace/MTRpc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/guofutan/workspace/MTRpc /home/guofutan/workspace/MTRpc/test /home/guofutan/workspace/MTRpc/build /home/guofutan/workspace/MTRpc/build/test /home/guofutan/workspace/MTRpc/build/test/CMakeFiles/test_tcpclient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test_tcpclient.dir/depend

