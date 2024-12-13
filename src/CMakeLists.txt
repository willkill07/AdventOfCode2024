target_compile_features(AdventOfCode2024 PUBLIC cxx_std_26)
target_include_directories(AdventOfCode2024 PRIVATE .)
target_sources(AdventOfCode2024
    PRIVATE
    advent.cpp)

target_sources(AdventOfCode2024
    PRIVATE
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
    spinner.cpp
    threading.cpp
    util.cpp)

target_link_libraries(AdventOfCode2024 PRIVATE ctre::ctre unordered_dense::unordered_dense dp::thread-pool)
