# CMake version
cmake_minimum_required (VERSION 3.15)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project declaration
project (
    vortice_libs
    VERSION 0.1.0
    DESCRIPTION "Various native library"
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add third party libraries
add_subdirectory(third_party)

# Add src
add_subdirectory(src)
