cmake_minimum_required(VERSION 3.16)
project(MiniC CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -g)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(minic ${SOURCES})