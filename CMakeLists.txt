cmake_minimum_required(VERSION 2.8)
project(reflectance)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

################################################################################
# Libraries
################################################################################


################################################################################
# Project settings
################################################################################

#set compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
endif ()

#define symbols for debug build
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG)
endif ()

#include directory of project
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

#project files to compile
file(GLOB_RECURSE project_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
file(GLOB_RECURSE project_HDR
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

#libraries to link
set(LINKED_LIBRARIES)

#clang stdlib
set(CLANG_STD_CXX
	c++
	c++abi
)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set( LINKED_LIBRARIES
		${LINKED_LIBRARIES}
		${CLANG_STD_CXX}
	)
endif ()

#set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_BINARY_DIR})

#generate completion database
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#build executable and then link to the libraries
add_executable(reflectance_test ${project_SRC} ${project_HDR}) 
