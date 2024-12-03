cmake_minimum_required(VERSION 3.28.0)

if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

project(AdventOfCode2024 CXX)

option(DEBUG Off)

if(DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  add_compile_options(-fsanitize=address -g)
  add_link_options(-fsanitize=address -g)
else()
  set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

FetchContent_Declare(
  ctre
  GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions
  GIT_TAG        v3.9.0
)

FetchContent_MakeAvailable(ctre)

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

add_subdirectory(src)
