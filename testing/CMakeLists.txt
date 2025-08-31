cmake_minimum_required(VERSION 3.20)

project(testing)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

add_executable(test src/main.cpp)

