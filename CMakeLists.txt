cmake_minimum_required(VERSION 3.20)
project(simulator)

set(CMAKE_CXX_STANDARD 20)
file(GLOB_RECURSE sources "src/*.cpp")

include_directories(include)

add_executable(code ${sources})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")