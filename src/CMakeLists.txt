target_compile_features(AdventOfCode2024 PUBLIC cxx_std_26)
target_include_directories(AdventOfCode2024 PRIVATE .)
target_sources(AdventOfCode2024
    PRIVATE
    advent.cpp
    util.cpp)
