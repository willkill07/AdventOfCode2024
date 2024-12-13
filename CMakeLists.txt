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
  GITHUB_REPOSITORY willkill07/thread-pool
  GIT_TAG 779cc0e4b92f7c32a89501c85f716b036d0b2668
  OPTIONS
    "TP_BUILD_TESTS OFF"
    "TP_BUILD_BENCHMARKS OFF"
    "TP_BUILD_EXAMPLES OFF"  
)

CPMAddPackage(
  NAME Catch2
  GITHUB_REPOSITORY catchorg/Catch2
  GIT_TAG v3.7.1
  OPTIONS
    "CATCH_INSTALL_DOCS OFF"
    "CATCH_INSTALL_EXTRAS OFF"
    "CATCH_ENABLE_REPRODUCIBLE_BUILD"
    "CATCH_BUILD_TESTING OFF"
    "CATCH_BUILD_EXAMPLES OFF"
    "CATCH_BUILD_EXTRA_TESTS OFF"
)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

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
add_executable(AdventOfCode2024Tests)
add_library(Advent)

enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(AdventOfCode2024Tests)

add_subdirectory(src)
