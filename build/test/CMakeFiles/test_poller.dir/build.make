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
include test/CMakeFiles/test_poller.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test_poller.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test_poller.dir/flags.make

test/CMakeFiles/test_poller.dir/test_poller.cpp.o: test/CMakeFiles/test_poller.dir/flags.make
test/CMakeFiles/test_poller.dir/test_poller.cpp.o: ../test/test_poller.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/guofutan/workspace/MTRpc/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/test_poller.dir/test_poller.cpp.o"
	cd /home/guofutan/workspace/MTRpc/build/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_poller.dir/test_poller.cpp.o -c /home/guofutan/workspace/MTRpc/test/test_poller.cpp

test/CMakeFiles/test_poller.dir/test_poller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_poller.dir/test_poller.cpp.i"
	cd /home/guofutan/workspace/MTRpc/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/guofutan/workspace/MTRpc/test/test_poller.cpp > CMakeFiles/test_poller.dir/test_poller.cpp.i

test/CMakeFiles/test_poller.dir/test_poller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_poller.dir/test_poller.cpp.s"
	cd /home/guofutan/workspace/MTRpc/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/guofutan/workspace/MTRpc/test/test_poller.cpp -o CMakeFiles/test_poller.dir/test_poller.cpp.s

test/CMakeFiles/test_poller.dir/test_poller.cpp.o.requires:
.PHONY : test/CMakeFiles/test_poller.dir/test_poller.cpp.o.requires

test/CMakeFiles/test_poller.dir/test_poller.cpp.o.provides: test/CMakeFiles/test_poller.dir/test_poller.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test_poller.dir/build.make test/CMakeFiles/test_poller.dir/test_poller.cpp.o.provides.build
.PHONY : test/CMakeFiles/test_poller.dir/test_poller.cpp.o.provides

test/CMakeFiles/test_poller.dir/test_poller.cpp.o.provides.build: test/CMakeFiles/test_poller.dir/test_poller.cpp.o

# Object files for target test_poller
test_poller_OBJECTS = \
"CMakeFiles/test_poller.dir/test_poller.cpp.o"

# External object files for target test_poller
test_poller_EXTERNAL_OBJECTS =

test/test_poller: test/CMakeFiles/test_poller.dir/test_poller.cpp.o
test/test_poller: test/CMakeFiles/test_poller.dir/build.make
test/test_poller: mio/libmio.a
test/test_poller: log/liblog.a
test/test_poller: common/libcommon.a
test/test_poller: proto/libproto.a
test/test_poller: thread/libthread.a
test/test_poller: json/libjson.a
test/test_poller: ../thirdparty/gtest/lib/libgtest.a
test/test_poller: test/CMakeFiles/test_poller.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_poller"
	cd /home/guofutan/workspace/MTRpc/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_poller.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test_poller.dir/build: test/test_poller
.PHONY : test/CMakeFiles/test_poller.dir/build

test/CMakeFiles/test_poller.dir/requires: test/CMakeFiles/test_poller.dir/test_poller.cpp.o.requires
.PHONY : test/CMakeFiles/test_poller.dir/requires

test/CMakeFiles/test_poller.dir/clean:
	cd /home/guofutan/workspace/MTRpc/build/test && $(CMAKE_COMMAND) -P CMakeFiles/test_poller.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test_poller.dir/clean

test/CMakeFiles/test_poller.dir/depend:
	cd /home/guofutan/workspace/MTRpc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/guofutan/workspace/MTRpc /home/guofutan/workspace/MTRpc/test /home/guofutan/workspace/MTRpc/build /home/guofutan/workspace/MTRpc/build/test /home/guofutan/workspace/MTRpc/build/test/CMakeFiles/test_poller.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test_poller.dir/depend

