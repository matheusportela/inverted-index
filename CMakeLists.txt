cmake_minimum_required(VERSION 2.8.12)
project(InvertedIndex)

# Configure output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Set compilation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -std=c++17 -g")

# Add include/ to the include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Compile all the .cpp files in src/
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Create executable files
add_executable(parse "${CMAKE_CURRENT_SOURCE_DIR}/src/cmd/parse.cpp" ${SOURCES})
add_executable(index "${CMAKE_CURRENT_SOURCE_DIR}/src/cmd/index.cpp" ${SOURCES})
add_executable(search "${CMAKE_CURRENT_SOURCE_DIR}/src/cmd/search.cpp" ${SOURCES})
