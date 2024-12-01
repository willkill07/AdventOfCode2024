#pragma once

#include "util.hpp"

#include <chrono>
#include <print>

#define SOLVE_DAY(Day)                                                         \
  do {                                                                         \
    using namespace Day;                                                       \
    util::buffer input{"inputs/" #Day ".txt"};                                 \
    auto t0 = std::chrono::steady_clock::now();                                \
    parsed const data = do_parse(input.span);                                  \
    auto t1 = std::chrono::steady_clock::now();                                \
    part1_answer const part1 = do_part1(data);                                 \
    auto t2 = std::chrono::steady_clock::now();                                \
    part2_answer const part2 = do_part2(data, part1);                          \
    auto t3 = std::chrono::steady_clock::now();                                \
    std::println("{}: Part1={} Part2={}", #Day, part1, part2);                 \
    auto parse_time =                                                          \
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count(); \
    auto p1_time =                                                             \
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count(); \
    auto p2_time =                                                             \
        std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count(); \
    auto total_time =                                                          \
        std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t0).count(); \
    std::println("       Parse={}ns Part1={}ns Part2={}ns Total={}ns",         \
                 parse_time, p1_time, p2_time, total_time);                    \
  } while (0)
