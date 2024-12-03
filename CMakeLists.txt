cmake_minimum_required(VERSION 3.28.0)

message(${CMAKE_HOST_SYSTEM_NAME})
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
  set(CMAKE_CXX_COMPILER g++-14)
endif()

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
