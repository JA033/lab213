# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dxy/Documents/csa_wildcard

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dxy/Documents/csa_wildcard/build

# Include any dependencies generated for this target.
include deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/compiler_depend.make

# Include the progress variables for this target.
include deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/progress.make

# Include the compile flags for this target's objects.
include deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/flags.make

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/flags.make
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o: ../deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/divsufsort.c
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dxy/Documents/csa_wildcard/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o -MF CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o.d -o CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o -c /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/divsufsort.c

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.i"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/divsufsort.c > CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.i

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.s"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/divsufsort.c -o CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.s

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/flags.make
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o: ../deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/sssort.c
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dxy/Documents/csa_wildcard/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o -MF CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o.d -o CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o -c /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/sssort.c

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.i"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/sssort.c > CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.i

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.s"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/sssort.c -o CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.s

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/flags.make
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o: ../deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/trsort.c
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dxy/Documents/csa_wildcard/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o -MF CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o.d -o CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o -c /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/trsort.c

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.i"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/trsort.c > CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.i

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.s"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/trsort.c -o CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.s

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/flags.make
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o: ../deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/utils.c
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dxy/Documents/csa_wildcard/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o -MF CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o.d -o CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o -c /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/utils.c

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/divsufsort64.dir/divsufsort/utils.c.i"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/utils.c > CMakeFiles/divsufsort64.dir/divsufsort/utils.c.i

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/divsufsort64.dir/divsufsort/utils.c.s"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64/divsufsort/utils.c -o CMakeFiles/divsufsort64.dir/divsufsort/utils.c.s

# Object files for target divsufsort64
divsufsort64_OBJECTS = \
"CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o" \
"CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o" \
"CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o" \
"CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o"

# External object files for target divsufsort64
divsufsort64_EXTERNAL_OBJECTS =

deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/divsufsort.c.o
deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/sssort.c.o
deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/trsort.c.o
deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/divsufsort/utils.c.o
deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/build.make
deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a: deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dxy/Documents/csa_wildcard/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C static library libdivsufsort64.a"
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && $(CMAKE_COMMAND) -P CMakeFiles/divsufsort64.dir/cmake_clean_target.cmake
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/divsufsort64.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/build: deps/GeCSA-Wrapper/source/GeCSA64/libdivsufsort64.a
.PHONY : deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/build

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/clean:
	cd /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 && $(CMAKE_COMMAND) -P CMakeFiles/divsufsort64.dir/cmake_clean.cmake
.PHONY : deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/clean

deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/depend:
	cd /home/dxy/Documents/csa_wildcard/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dxy/Documents/csa_wildcard /home/dxy/Documents/csa_wildcard/deps/GeCSA-Wrapper/source/GeCSA64 /home/dxy/Documents/csa_wildcard/build /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64 /home/dxy/Documents/csa_wildcard/build/deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : deps/GeCSA-Wrapper/source/GeCSA64/CMakeFiles/divsufsort64.dir/depend

