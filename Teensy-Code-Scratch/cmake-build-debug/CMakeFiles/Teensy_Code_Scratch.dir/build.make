# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Teensy_Code_Scratch.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/Teensy_Code_Scratch.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Teensy_Code_Scratch.dir/flags.make

CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.o: CMakeFiles/Teensy_Code_Scratch.dir/flags.make
CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.o -c /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/main.cpp

CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/main.cpp > CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.i

CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/main.cpp -o CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.s

# Object files for target Teensy_Code_Scratch
Teensy_Code_Scratch_OBJECTS = \
"CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.o"

# External object files for target Teensy_Code_Scratch
Teensy_Code_Scratch_EXTERNAL_OBJECTS =

Teensy_Code_Scratch: CMakeFiles/Teensy_Code_Scratch.dir/main.cpp.o
Teensy_Code_Scratch: CMakeFiles/Teensy_Code_Scratch.dir/build.make
Teensy_Code_Scratch: CMakeFiles/Teensy_Code_Scratch.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Teensy_Code_Scratch"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Teensy_Code_Scratch.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Teensy_Code_Scratch.dir/build: Teensy_Code_Scratch
.PHONY : CMakeFiles/Teensy_Code_Scratch.dir/build

CMakeFiles/Teensy_Code_Scratch.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Teensy_Code_Scratch.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Teensy_Code_Scratch.dir/clean

CMakeFiles/Teensy_Code_Scratch.dir/depend:
	cd /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug /Users/shawn/Documents/GitHub/Inflight-Arduino-Code/Teensy-Code-Scratch/cmake-build-debug/CMakeFiles/Teensy_Code_Scratch.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Teensy_Code_Scratch.dir/depend

