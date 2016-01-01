# Set the name and the supported language of the project
project(MC_GUI)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/tim/ClionProjects/MC_GUI")
# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 3.3)
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})
# Add an executable compiled from hello.c
add_executable(mc_gui mc_gui.c mc_gui.h util.c util.h)
# Link the target to the GTK+ libraries
target_link_libraries(mc_gui ${GTK3_LIBRARIES})

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

#set(SOURCE_FILES mc_gui.c)
#add_executable(MC_GUI ${SOURCE_FILES})
