# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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
CMAKE_SOURCE_DIR = /home/linghutf/asio/async

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/linghutf/asio/async

# Include any dependencies generated for this target.
include CMakeFiles/server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/server.dir/flags.make

CMakeFiles/server.dir/server.cc.o: CMakeFiles/server.dir/flags.make
CMakeFiles/server.dir/server.cc.o: server.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/linghutf/asio/async/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/server.dir/server.cc.o"
	/usr/bin/clang++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/server.dir/server.cc.o -c /home/linghutf/asio/async/server.cc

CMakeFiles/server.dir/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/server.cc.i"
	/usr/bin/clang++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/linghutf/asio/async/server.cc > CMakeFiles/server.dir/server.cc.i

CMakeFiles/server.dir/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/server.cc.s"
	/usr/bin/clang++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/linghutf/asio/async/server.cc -o CMakeFiles/server.dir/server.cc.s

CMakeFiles/server.dir/server.cc.o.requires:
.PHONY : CMakeFiles/server.dir/server.cc.o.requires

CMakeFiles/server.dir/server.cc.o.provides: CMakeFiles/server.dir/server.cc.o.requires
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/server.cc.o.provides.build
.PHONY : CMakeFiles/server.dir/server.cc.o.provides

CMakeFiles/server.dir/server.cc.o.provides.build: CMakeFiles/server.dir/server.cc.o

CMakeFiles/server.dir/fileinfo.cc.o: CMakeFiles/server.dir/flags.make
CMakeFiles/server.dir/fileinfo.cc.o: fileinfo.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/linghutf/asio/async/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/server.dir/fileinfo.cc.o"
	/usr/bin/clang++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/server.dir/fileinfo.cc.o -c /home/linghutf/asio/async/fileinfo.cc

CMakeFiles/server.dir/fileinfo.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/fileinfo.cc.i"
	/usr/bin/clang++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/linghutf/asio/async/fileinfo.cc > CMakeFiles/server.dir/fileinfo.cc.i

CMakeFiles/server.dir/fileinfo.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/fileinfo.cc.s"
	/usr/bin/clang++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/linghutf/asio/async/fileinfo.cc -o CMakeFiles/server.dir/fileinfo.cc.s

CMakeFiles/server.dir/fileinfo.cc.o.requires:
.PHONY : CMakeFiles/server.dir/fileinfo.cc.o.requires

CMakeFiles/server.dir/fileinfo.cc.o.provides: CMakeFiles/server.dir/fileinfo.cc.o.requires
	$(MAKE) -f CMakeFiles/server.dir/build.make CMakeFiles/server.dir/fileinfo.cc.o.provides.build
.PHONY : CMakeFiles/server.dir/fileinfo.cc.o.provides

CMakeFiles/server.dir/fileinfo.cc.o.provides.build: CMakeFiles/server.dir/fileinfo.cc.o

# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/server.cc.o" \
"CMakeFiles/server.dir/fileinfo.cc.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

server: CMakeFiles/server.dir/server.cc.o
server: CMakeFiles/server.dir/fileinfo.cc.o
server: CMakeFiles/server.dir/build.make
server: /usr/lib/x86_64-linux-gnu/libboost_system.so
server: /usr/lib/x86_64-linux-gnu/libboost_thread.so
server: /usr/lib/x86_64-linux-gnu/libboost_log.so
server: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
server: /usr/lib/x86_64-linux-gnu/libpthread.so
server: CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/server.dir/build: server
.PHONY : CMakeFiles/server.dir/build

CMakeFiles/server.dir/requires: CMakeFiles/server.dir/server.cc.o.requires
CMakeFiles/server.dir/requires: CMakeFiles/server.dir/fileinfo.cc.o.requires
.PHONY : CMakeFiles/server.dir/requires

CMakeFiles/server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/server.dir/clean

CMakeFiles/server.dir/depend:
	cd /home/linghutf/asio/async && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/linghutf/asio/async /home/linghutf/asio/async /home/linghutf/asio/async /home/linghutf/asio/async /home/linghutf/asio/async/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/server.dir/depend

