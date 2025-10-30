cmake_minimum_required(VERSION 3.20)
project(testing)

set(CMAKE_CXX_STANDARD 20)

# Enable profiling and debug info
# -g = debug symbols, -pg = enable gprof profiling and -02 = compiler optimizatinos 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg -O2") # -O2 optional, adds compiler optimizations

# Source files (header-only generators are included via main.cpp)
add_executable(testy 
    src/main.cpp
    src/tokenization.hpp
    src/parser.hpp
    src/generation.hpp
)
