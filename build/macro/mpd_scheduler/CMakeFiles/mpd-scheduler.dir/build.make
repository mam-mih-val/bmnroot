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
CMAKE_COMMAND = /opt/fairsoft/install/bin/cmake

# The command to remove a file.
RM = /opt/fairsoft/install/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /opt/fairsoft/install/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/merz/BMN/GIT/trunk_220814

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/merz/BMN/GIT/trunk_220814/build

# Include any dependencies generated for this target.
include macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/depend.make

# Include the progress variables for this target.
include macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/progress.make

# Include the compile flags for this target's objects.
include macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/flags.make

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/flags.make
macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o: ../macro/mpd_scheduler/src/mpd-scheduler.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/merz/BMN/GIT/trunk_220814/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o"
	cd /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o -c /home/merz/BMN/GIT/trunk_220814/macro/mpd_scheduler/src/mpd-scheduler.cpp

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.i"
	cd /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/merz/BMN/GIT/trunk_220814/macro/mpd_scheduler/src/mpd-scheduler.cpp > CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.i

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.s"
	cd /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/merz/BMN/GIT/trunk_220814/macro/mpd_scheduler/src/mpd-scheduler.cpp -o CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.s

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.requires:
.PHONY : macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.requires

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.provides: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.requires
	$(MAKE) -f macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/build.make macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.provides.build
.PHONY : macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.provides

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.provides.build: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o

# Object files for target mpd-scheduler
mpd__scheduler_OBJECTS = \
"CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o"

# External object files for target mpd-scheduler
mpd__scheduler_EXTERNAL_OBJECTS =

bin/mpd-scheduler: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o
bin/mpd-scheduler: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/build.make
bin/mpd-scheduler: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/mpd-scheduler"
	cd /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mpd-scheduler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/build: bin/mpd-scheduler
.PHONY : macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/build

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/requires: macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/src/mpd-scheduler.cpp.o.requires
.PHONY : macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/requires

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/clean:
	cd /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler && $(CMAKE_COMMAND) -P CMakeFiles/mpd-scheduler.dir/cmake_clean.cmake
.PHONY : macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/clean

macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/depend:
	cd /home/merz/BMN/GIT/trunk_220814/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/merz/BMN/GIT/trunk_220814 /home/merz/BMN/GIT/trunk_220814/macro/mpd_scheduler /home/merz/BMN/GIT/trunk_220814/build /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler /home/merz/BMN/GIT/trunk_220814/build/macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : macro/mpd_scheduler/CMakeFiles/mpd-scheduler.dir/depend

