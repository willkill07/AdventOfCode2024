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
    util.cpp)

target_link_libraries(AdventOfCode2024 PRIVATE ctre::ctre)
