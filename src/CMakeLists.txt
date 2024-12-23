target_compile_features(Advent PUBLIC cxx_std_26)
target_include_directories(Advent PRIVATE .)
target_sources(Advent
    PUBLIC
    FILE_SET cxx_modules
    TYPE CXX_MODULES
    FILES
    day01.cpp
    day02.cpp
    day03.cpp
    day04.cpp
    day05.cpp
    day06.cpp
    day07.cpp
    day08.cpp
    day09.cpp
    day10.cpp
    day11.cpp
    day12.cpp
    day13.cpp
    day14.cpp
    day15.cpp
    day16.cpp
    day17.cpp
    day18.cpp
    day19.cpp
    day20.cpp
    day21.cpp
    day22.cpp
    spinner.cpp
    threading.cpp
    util.cpp)

target_link_libraries(Advent PRIVATE ctre::ctre unordered_dense::unordered_dense dp::thread-pool)

target_compile_features(AdventOfCode2024 PUBLIC cxx_std_26)
target_sources(AdventOfCode2024
    PRIVATE
    advent.cpp)
target_link_libraries(AdventOfCode2024 PRIVATE Advent)

target_compile_features(AdventOfCode2024Tests PUBLIC cxx_std_26)
target_sources(AdventOfCode2024Tests
    PRIVATE
    tests.cpp)
target_link_libraries(AdventOfCode2024Tests PRIVATE Advent Catch2::Catch2WithMain)

target_compile_features(AdventOfCode2024Benchmarks PUBLIC cxx_std_26)
target_sources(AdventOfCode2024Benchmarks
    PRIVATE
    benchmarks.cpp)
target_link_libraries(AdventOfCode2024Benchmarks PRIVATE Advent Catch2::Catch2WithMain)
