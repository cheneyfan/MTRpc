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
CMAKE_SOURCE_DIR = /home/guofutan/workspace/moduleframework

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/guofutan/workspace/moduleframework/build

# Include any dependencies generated for this target.
include test/CMakeFiles/test_quue.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test_quue.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test_quue.dir/flags.make

test/CMakeFiles/test_quue.dir/test_quue.cpp.o: test/CMakeFiles/test_quue.dir/flags.make
test/CMakeFiles/test_quue.dir/test_quue.cpp.o: ../test/test_quue.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/guofutan/workspace/moduleframework/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/test_quue.dir/test_quue.cpp.o"
	cd /home/guofutan/workspace/moduleframework/build/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_quue.dir/test_quue.cpp.o -c /home/guofutan/workspace/moduleframework/test/test_quue.cpp

test/CMakeFiles/test_quue.dir/test_quue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_quue.dir/test_quue.cpp.i"
	cd /home/guofutan/workspace/moduleframework/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/guofutan/workspace/moduleframework/test/test_quue.cpp > CMakeFiles/test_quue.dir/test_quue.cpp.i

test/CMakeFiles/test_quue.dir/test_quue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_quue.dir/test_quue.cpp.s"
	cd /home/guofutan/workspace/moduleframework/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/guofutan/workspace/moduleframework/test/test_quue.cpp -o CMakeFiles/test_quue.dir/test_quue.cpp.s

test/CMakeFiles/test_quue.dir/test_quue.cpp.o.requires:
.PHONY : test/CMakeFiles/test_quue.dir/test_quue.cpp.o.requires

test/CMakeFiles/test_quue.dir/test_quue.cpp.o.provides: test/CMakeFiles/test_quue.dir/test_quue.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test_quue.dir/build.make test/CMakeFiles/test_quue.dir/test_quue.cpp.o.provides.build
.PHONY : test/CMakeFiles/test_quue.dir/test_quue.cpp.o.provides

test/CMakeFiles/test_quue.dir/test_quue.cpp.o.provides.build: test/CMakeFiles/test_quue.dir/test_quue.cpp.o

# Object files for target test_quue
test_quue_OBJECTS = \
"CMakeFiles/test_quue.dir/test_quue.cpp.o"

# External object files for target test_quue
test_quue_EXTERNAL_OBJECTS =

test/test_quue: test/CMakeFiles/test_quue.dir/test_quue.cpp.o
test/test_quue: test/CMakeFiles/test_quue.dir/build.make
test/test_quue: mio2/libmio2.a
test/test_quue: libjson/libjson.a
test/test_quue: ../thirdparty/gtest/lib/libgtest.a
test/test_quue: test/CMakeFiles/test_quue.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_quue"
	cd /home/guofutan/workspace/moduleframework/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_quue.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test_quue.dir/build: test/test_quue
.PHONY : test/CMakeFiles/test_quue.dir/build

test/CMakeFiles/test_quue.dir/requires: test/CMakeFiles/test_quue.dir/test_quue.cpp.o.requires
.PHONY : test/CMakeFiles/test_quue.dir/requires

test/CMakeFiles/test_quue.dir/clean:
	cd /home/guofutan/workspace/moduleframework/build/test && $(CMAKE_COMMAND) -P CMakeFiles/test_quue.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test_quue.dir/clean

test/CMakeFiles/test_quue.dir/depend:
	cd /home/guofutan/workspace/moduleframework/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/guofutan/workspace/moduleframework /home/guofutan/workspace/moduleframework/test /home/guofutan/workspace/moduleframework/build /home/guofutan/workspace/moduleframework/build/test /home/guofutan/workspace/moduleframework/build/test/CMakeFiles/test_quue.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test_quue.dir/depend

