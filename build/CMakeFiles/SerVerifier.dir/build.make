# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /workspace/Vbox

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /workspace/Vbox/build

# Include any dependencies generated for this target.
include CMakeFiles/SerVerifier.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SerVerifier.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SerVerifier.dir/flags.make

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o: ../src/transaction/transaction.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o -c /workspace/Vbox/src/transaction/transaction.cpp

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/transaction/transaction.cpp > CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.i

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/transaction/transaction.cpp -o CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.s

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o


CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o: ../src/graph/graph.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o -c /workspace/Vbox/src/graph/graph.cpp

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/graph/graph.cpp > CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.i

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/graph/graph.cpp -o CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.s

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o


CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o: ../src/graph/transitive_closure.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o -c /workspace/Vbox/src/graph/transitive_closure.cpp

CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/graph/transitive_closure.cpp > CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.i

CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/graph/transitive_closure.cpp -o CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.s

CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o


CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o: ../src/verifier/vbox.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o -c /workspace/Vbox/src/verifier/vbox.cpp

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/verifier/vbox.cpp > CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.i

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/verifier/vbox.cpp -o CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.s

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o


CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o: ../src/verifier/leopard.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o -c /workspace/Vbox/src/verifier/leopard.cpp

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/verifier/leopard.cpp > CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.i

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/verifier/leopard.cpp -o CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.s

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o


CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o: ../src/solver/constraint.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o -c /workspace/Vbox/src/solver/constraint.cpp

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/solver/constraint.cpp > CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.i

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/solver/constraint.cpp -o CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.s

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o


CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o: ../src/solver/solver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o -c /workspace/Vbox/src/solver/solver.cpp

CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/solver/solver.cpp > CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.i

CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/solver/solver.cpp -o CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.s

CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o


CMakeFiles/SerVerifier.dir/src/main.cpp.o: CMakeFiles/SerVerifier.dir/flags.make
CMakeFiles/SerVerifier.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/SerVerifier.dir/src/main.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SerVerifier.dir/src/main.cpp.o -c /workspace/Vbox/src/main.cpp

CMakeFiles/SerVerifier.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SerVerifier.dir/src/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /workspace/Vbox/src/main.cpp > CMakeFiles/SerVerifier.dir/src/main.cpp.i

CMakeFiles/SerVerifier.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SerVerifier.dir/src/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /workspace/Vbox/src/main.cpp -o CMakeFiles/SerVerifier.dir/src/main.cpp.s

CMakeFiles/SerVerifier.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/SerVerifier.dir/src/main.cpp.o.requires

CMakeFiles/SerVerifier.dir/src/main.cpp.o.provides: CMakeFiles/SerVerifier.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/SerVerifier.dir/build.make CMakeFiles/SerVerifier.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/SerVerifier.dir/src/main.cpp.o.provides

CMakeFiles/SerVerifier.dir/src/main.cpp.o.provides.build: CMakeFiles/SerVerifier.dir/src/main.cpp.o


# Object files for target SerVerifier
SerVerifier_OBJECTS = \
"CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o" \
"CMakeFiles/SerVerifier.dir/src/main.cpp.o"

# External object files for target SerVerifier
SerVerifier_EXTERNAL_OBJECTS =

SerVerifier: CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/src/main.cpp.o
SerVerifier: CMakeFiles/SerVerifier.dir/build.make
SerVerifier: CMakeFiles/SerVerifier.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/workspace/Vbox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking CXX executable SerVerifier"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SerVerifier.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SerVerifier.dir/build: SerVerifier

.PHONY : CMakeFiles/SerVerifier.dir/build

CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/transaction/transaction.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/graph/graph.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/graph/transitive_closure.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/verifier/vbox.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/verifier/leopard.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/solver/constraint.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/solver/solver.cpp.o.requires
CMakeFiles/SerVerifier.dir/requires: CMakeFiles/SerVerifier.dir/src/main.cpp.o.requires

.PHONY : CMakeFiles/SerVerifier.dir/requires

CMakeFiles/SerVerifier.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SerVerifier.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SerVerifier.dir/clean

CMakeFiles/SerVerifier.dir/depend:
	cd /workspace/Vbox/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /workspace/Vbox /workspace/Vbox /workspace/Vbox/build /workspace/Vbox/build /workspace/Vbox/build/CMakeFiles/SerVerifier.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SerVerifier.dir/depend

