# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /opt/conda/lib/python3.8/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /opt/conda/lib/python3.8/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /workspace/verifier

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /workspace/verifier/build

# Include any dependencies generated for this target.
include CMakeFiles/SerVerifier.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/SerVerifier.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/SerVerifier.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SerVerifier.dir/flags.make

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o: /workspace/verifier/src/transaction/transaction.cpp
CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o: CMakeFiles/SerVerifier.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o -MF CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.d -o CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o -c /workspace/verifier/src/transaction/transaction.cpp

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/verifier/src/transaction/transaction.cpp > CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.i

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/verifier/src/transaction/transaction.cpp -o CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.s

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o: /workspace/verifier/src/graph/graph.cpp
CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o: CMakeFiles/SerVerifier.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o -MF CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.d -o CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o -c /workspace/verifier/src/graph/graph.cpp

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/verifier/src/graph/graph.cpp > CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.i

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/verifier/src/graph/graph.cpp -o CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.s

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o: /workspace/verifier/src/verifier/vbox.cpp
CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o: CMakeFiles/SerVerifier.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o -MF CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.d -o CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o -c /workspace/verifier/src/verifier/vbox.cpp

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/verifier/src/verifier/vbox.cpp > CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.i

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/verifier/src/verifier/vbox.cpp -o CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.s

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o: /workspace/verifier/src/verifier/leopard.cpp
CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o: CMakeFiles/SerVerifier.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o -MF CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.d -o CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o -c /workspace/verifier/src/verifier/leopard.cpp

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/verifier/src/verifier/leopard.cpp > CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.i

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/verifier/src/verifier/leopard.cpp -o CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.s

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o: /workspace/verifier/src/solver/constraint.cpp
CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o: CMakeFiles/SerVerifier.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o -MF CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.d -o CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o -c /workspace/verifier/src/solver/constraint.cpp

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/verifier/src/solver/constraint.cpp > CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.i

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/verifier/src/solver/constraint.cpp -o CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.s

CMakeFiles/SerVerifier.dir/src/main.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/main.cpp.o: /workspace/verifier/src/main.cpp
CMakeFiles/SerVerifier.dir/src/main.cpp.o: CMakeFiles/SerVerifier.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/SerVerifier.dir/src/main.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SerVerifier.dir/src/main.cpp.o -MF CMakeFiles/SerVerifier.dir/src/main.cpp.o.d -o CMakeFiles/SerVerifier.dir/src/main.cpp.o -c /workspace/verifier/src/main.cpp

CMakeFiles/SerVerifier.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/verifier/src/main.cpp > CMakeFiles/SerVerifier.dir/src/main.cpp.i

CMakeFiles/SerVerifier.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/verifier/src/main.cpp -o CMakeFiles/SerVerifier.dir/src/main.cpp.s

# Object files for target SerVerifier
SerVerifier_OBJECTS = \
"CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/main.cpp.o"

# External object files for target SerVerifier
SerVerifier_EXTERNAL_OBJECTS =

SerVerifier: CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/main.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/build.make
SerVerifier: CMakeFiles/SerVerifier.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/workspace/verifier/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable SerVerifier"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SerVerifier.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SerVerifier.dir/build: SerVerifier
.PHONY : CMakeFiles/SerVerifier.dir/build

CMakeFiles/SerVerifier.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SerVerifier.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SerVerifier.dir/clean

CMakeFiles/SerVerifier.dir/depend:
	cd /workspace/verifier/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /workspace/verifier /workspace/verifier /workspace/verifier/build /workspace/verifier/build /workspace/verifier/build/CMakeFiles/SerVerifier.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SerVerifier.dir/depend

