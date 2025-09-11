cmake_minimum_required(VERSION 3.20)
project(testing)

set(CMAKE_CXX_STANDARD 20)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")

# Include LLVM headers
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


# Source files (header-only generators are included via main.cpp)
add_executable(testy 
    src/main.cpp
    src/tokenization.hpp
    src/parser.hpp
    src/generation.hpp
    src/llvm_generator.hpp
)


# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs core irreader support)
target_link_libraries(testy ${llvm_libs})