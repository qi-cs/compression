cmake_minimum_required(VERSION 3.10)
project(compression_lib)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set build type to Debug
set(CMAKE_BUILD_TYPE Debug)
# Create library
add_library(compression
    src/cpack.cc
    src/bdi.cc
    src/fpc.cc
    src/lz4.cc
    src/huffman.cc
)

# Set include directories
target_include_directories(compression
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add tests
add_subdirectory(tests) 