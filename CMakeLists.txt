cmake_minimum_required(VERSION 3.26.0)
project(AdventOfCode2024 CXX)

if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

option(DEBUG Off)

if(DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  add_compile_options(-fsanitize=address -g)
  add_link_options(-fsanitize=address -g)
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wuninitialized
    -Wshadow
    -march=native
    $<$<CXX_COMPILER_ID:GNU>:-Wnrvo>)

add_executable(AdventOfCode2024)
target_compile_features(AdventOfCode2024 PUBLIC cxx_std_26)
target_sources(AdventOfCode2024
    PRIVATE
    advent.cpp
    util.cpp)
