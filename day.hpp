#pragma once

#include "util.hpp"

#include <chrono>
#include <cstring>
#include <format>

template <auto ParseFn, auto Part1Fn, auto Part2Fn>
void solve_day(char const *day, char const *filename) {
  char line[120];
  std::memset(line, 0, 120);
  util::buffer input{filename};
  auto t0 = std::chrono::steady_clock::now();
  auto const data = ParseFn(input.span);
  auto t1 = std::chrono::steady_clock::now();
  auto const part1 = Part1Fn(data);
  auto t2 = std::chrono::steady_clock::now();
  auto const part2 = Part2Fn(data, part1);
  auto t3 = std::chrono::steady_clock::now();
  std::format_to(line, "{}: Part1={} Part2={}\n\0", day, part1, part2);
  fprintf(stdout, "%s", line);
  auto parse_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  auto p1_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
  auto p2_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count();
  auto total_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t0).count();
  std::format_to(line, "       Parse={}ns Part1={}ns Part2={}ns Total={}ns\n\0",
                 parse_time, p1_time, p2_time, total_time);
  fprintf(stdout, "%s", line);
}

#define SOLVE_DAY(Day)                                                         \
  solve_day<&Day##_parse, &Day##_part1, &Day##_part2>(#Day,                    \
                                                      "inputs/" #Day ".txt")
