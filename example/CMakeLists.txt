
cmake_minimum_required(VERSION 3.6.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(
    example
    VERSION 0.1
    DESCRIPTION "An example program for the druck rendering library."
    LANGUAGES CXX
)

find_package(raylib REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE DRUCK_SOURCES "./../src/*.cpp")
add_executable(
    example
    ${SOURCES}
    ${DRUCK_SOURCES}
)

target_include_directories(example PRIVATE "./../include")
target_link_libraries(example raylib)

target_compile_features(example PRIVATE cxx_std_20)
target_compile_options(example PRIVATE -Wall -Wextra -Wpedantic -O3 -funroll-loops -flto -ffast-math -ftree-vectorize)