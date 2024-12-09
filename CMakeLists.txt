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

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME ctre
  GITHUB_REPOSITORY hanickadot/compile-time-regular-expressions
  GIT_TAG v3.9.0
)

CPMAddPackage(
  NAME unordered_dense
  GITHUB_REPOSITORY martinus/unordered_dense
  GIT_TAG v4.5.0
)

CPMAddPackage(
  NAME thread-pool
  GITHUB_REPOSITORY DeveloperPaul123/thread-pool
  GIT_TAG bc107644b3b8605b8ea6a4dbd25b35fa304ebebb
  OPTIONS
    "TP_BUILD_TESTS OFF"
    "TP_BUILD_BENCHMARKS OFF"
    "TP_BUILD_EXAMPLES OFF"  
)

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
