cmake_minimum_required(VERSION 3.10)
project(x86_compiler_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the FetchContent module to download Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories for the compiler project
include_directories(${CMAKE_SOURCE_DIR}/../src) # Adjust this path based on your project structure

# Add the test executable
add_executable(compiler_tests compiler_test.cpp)

# Link against Google Test and any other necessary libraries
target_link_libraries(compiler_tests 
    gtest_main
    # Add your compiler library here if applicable, e.g.:
    # x86_compiler_lib
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(compiler_tests)