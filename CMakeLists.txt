# CMake version
cmake_minimum_required (VERSION 3.14)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

# Options
if (IOS OR EMSCRIPTEN)
     set(BUILD_SHARED_LIBS OFF CACHE BOOL "Always Disable shared library on (IOS, WEB)" FORCE)
else()
    option(BUILD_SHARED_LIBS "Build shared library" ON)
endif()

# Configure CMake global variables
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_INSTALL_MESSAGE LAZY)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project declaration
project (vortice_libs LANGUAGES CXX C)

# Configure CMake global variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Version
SET(LIB_MAJOR_VERSION "1")
SET(LIB_MINOR_VERSION "0")
SET(LIB_REVISION "0")
SET(LIB_VERSION "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_REVISION}")

# Build Type
if(NOT CMAKE_BUILD_TYPE)
	# By default, we use Release
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Platform Flags
if(APPLE)
	set(CMAKE_MACOSX_RPATH ON)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
	set(LOBJC "objc")
elseif(WIN32)
	# "VorticeImage.dll", not "libVorticeImage.dll"
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

# Add third party libraries
add_subdirectory(third_party)

# Add src
add_subdirectory(src)
