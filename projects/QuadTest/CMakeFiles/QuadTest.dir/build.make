# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/fredr/source/ltu-lab-s0006e_env

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fredr/source/ltu-lab-s0006e_env

# Include any dependencies generated for this target.
include projects/QuadTest/CMakeFiles/QuadTest.dir/depend.make

# Include the progress variables for this target.
include projects/QuadTest/CMakeFiles/QuadTest.dir/progress.make

# Include the compile flags for this target's objects.
include projects/QuadTest/CMakeFiles/QuadTest.dir/flags.make

projects/QuadTest/CMakeFiles/QuadTest.dir/code/main.cc.o: projects/QuadTest/CMakeFiles/QuadTest.dir/flags.make
projects/QuadTest/CMakeFiles/QuadTest.dir/code/main.cc.o: projects/QuadTest/code/main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fredr/source/ltu-lab-s0006e_env/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/QuadTest/CMakeFiles/QuadTest.dir/code/main.cc.o"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QuadTest.dir/code/main.cc.o -c /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/code/main.cc

projects/QuadTest/CMakeFiles/QuadTest.dir/code/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuadTest.dir/code/main.cc.i"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/code/main.cc > CMakeFiles/QuadTest.dir/code/main.cc.i

projects/QuadTest/CMakeFiles/QuadTest.dir/code/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuadTest.dir/code/main.cc.s"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/code/main.cc -o CMakeFiles/QuadTest.dir/code/main.cc.s

projects/QuadTest/CMakeFiles/QuadTest.dir/code/quadtest.cc.o: projects/QuadTest/CMakeFiles/QuadTest.dir/flags.make
projects/QuadTest/CMakeFiles/QuadTest.dir/code/quadtest.cc.o: projects/QuadTest/code/quadtest.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fredr/source/ltu-lab-s0006e_env/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object projects/QuadTest/CMakeFiles/QuadTest.dir/code/quadtest.cc.o"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QuadTest.dir/code/quadtest.cc.o -c /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/code/quadtest.cc

projects/QuadTest/CMakeFiles/QuadTest.dir/code/quadtest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuadTest.dir/code/quadtest.cc.i"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/code/quadtest.cc > CMakeFiles/QuadTest.dir/code/quadtest.cc.i

projects/QuadTest/CMakeFiles/QuadTest.dir/code/quadtest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuadTest.dir/code/quadtest.cc.s"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/code/quadtest.cc -o CMakeFiles/QuadTest.dir/code/quadtest.cc.s

# Object files for target QuadTest
QuadTest_OBJECTS = \
"CMakeFiles/QuadTest.dir/code/main.cc.o" \
"CMakeFiles/QuadTest.dir/code/quadtest.cc.o"

# External object files for target QuadTest
QuadTest_EXTERNAL_OBJECTS =

bin/QuadTest: projects/QuadTest/CMakeFiles/QuadTest.dir/code/main.cc.o
bin/QuadTest: projects/QuadTest/CMakeFiles/QuadTest.dir/code/quadtest.cc.o
bin/QuadTest: projects/QuadTest/CMakeFiles/QuadTest.dir/build.make
bin/QuadTest: engine/core/libcore.a
bin/QuadTest: engine/render/librender.a
bin/QuadTest: projects/MeshResource/libMeshResource.a
bin/QuadTest: projects/Rasterizer/libRasterizer.a
bin/QuadTest: projects/MeshResource/libMeshResource.a
bin/QuadTest: projects/VectorLib/libVectorLib.a
bin/QuadTest: engine/core/libcore.a
bin/QuadTest: engine/render/librender.a
bin/QuadTest: engine/core/libcore.a
bin/QuadTest: engine/render/librender.a
bin/QuadTest: exts/glew/libglew.a
bin/QuadTest: exts/glfw/src/libglfw3.a
bin/QuadTest: /usr/lib/librt.so
bin/QuadTest: /usr/lib/libm.so
bin/QuadTest: /usr/lib/libX11.so
bin/QuadTest: /usr/lib/libXrandr.so
bin/QuadTest: /usr/lib/libXinerama.so
bin/QuadTest: /usr/lib/libXxf86vm.so
bin/QuadTest: /usr/lib/libXcursor.so
bin/QuadTest: exts/libimgui.a
bin/QuadTest: exts/libstb.a
bin/QuadTest: exts/libtinygltf.a
bin/QuadTest: exts/libimgui.a
bin/QuadTest: exts/libstb.a
bin/QuadTest: exts/libtinygltf.a
bin/QuadTest: projects/QuadTest/CMakeFiles/QuadTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fredr/source/ltu-lab-s0006e_env/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../bin/QuadTest"
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/QuadTest.dir/link.txt --verbose=$(VERBOSE)
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && /usr/bin/cmake -E copy_directory /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/res/ /home/fredr/source/ltu-lab-s0006e_env/bin/res/

# Rule to build all files generated by this target.
projects/QuadTest/CMakeFiles/QuadTest.dir/build: bin/QuadTest

.PHONY : projects/QuadTest/CMakeFiles/QuadTest.dir/build

projects/QuadTest/CMakeFiles/QuadTest.dir/clean:
	cd /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest && $(CMAKE_COMMAND) -P CMakeFiles/QuadTest.dir/cmake_clean.cmake
.PHONY : projects/QuadTest/CMakeFiles/QuadTest.dir/clean

projects/QuadTest/CMakeFiles/QuadTest.dir/depend:
	cd /home/fredr/source/ltu-lab-s0006e_env && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fredr/source/ltu-lab-s0006e_env /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest /home/fredr/source/ltu-lab-s0006e_env /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest /home/fredr/source/ltu-lab-s0006e_env/projects/QuadTest/CMakeFiles/QuadTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : projects/QuadTest/CMakeFiles/QuadTest.dir/depend

