cmake_minimum_required(VERSION 3.10)
project(DitheringEngine VERSION 0.1 LANGUAGES C CXX)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(external)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Visual Studio msvc glich: not using c++17 standard
