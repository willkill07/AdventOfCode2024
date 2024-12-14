#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

import util;
import day01;
import day02;
import day03;
import day04;
import day05;
import day06;
import day07;
import day08;
import day09;
import day10;
import day11;
import day12;
import day13;

#define EMIT_BENCHMARK(DayNum) \
  TEST_CASE(#DayNum " Benchmark") { \
    std::string file = util::ReadFile("inputs/" #DayNum ".txt"); \
    BENCHMARK("File I/O") { std::ignore = util::ReadFile("inputs/" #DayNum ".txt"); }; \
    decltype(auto) parsed = DayNum ## Parse(file); \
    BENCHMARK("Parse") { std::ignore = DayNum ## Parse(file); }; \
    decltype(auto) part1 = DayNum ## Part1(parsed); \
    BENCHMARK("Part1") { std::ignore = DayNum ## Part1(parsed); }; \
    BENCHMARK("Part2") { std::ignore = DayNum ## Part2(parsed, part1); }; \
  }

EMIT_BENCHMARK(Day01);
EMIT_BENCHMARK(Day02);
EMIT_BENCHMARK(Day03);
EMIT_BENCHMARK(Day04);
EMIT_BENCHMARK(Day05);
EMIT_BENCHMARK(Day06);
EMIT_BENCHMARK(Day07);
EMIT_BENCHMARK(Day08);
EMIT_BENCHMARK(Day09);
EMIT_BENCHMARK(Day10);
EMIT_BENCHMARK(Day11);
EMIT_BENCHMARK(Day12);
