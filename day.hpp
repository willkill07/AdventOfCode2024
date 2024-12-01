#pragma once

#include "timing.hpp"
#include "util.hpp"

#include <chrono>
#include <print>

template <auto ParseFn, auto Part1Fn, auto Part2Fn>
[[nodiscard]] [[gnu::noinline]] static timing_stats
solve_day(char const *day, char const *filename) noexcept {
  using clock_type = std::chrono::steady_clock;
  util::buffer input{filename};
  clock_type::time_point const t0 = clock_type::now();
  auto const data = ParseFn(input.span);
  clock_type::time_point const t1 = clock_type::now();
  auto const part1 = Part1Fn(data);
  clock_type::time_point const t2 = clock_type::now();
  auto const part2 = Part2Fn(data, part1);
  clock_type::time_point const t3 = clock_type::now();
  timing_stats const stats{
      .parse = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0)
                   .count(),
      .part1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1)
                   .count(),
      .part2 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2)
                   .count(),
      .total = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t0)
                   .count()};
  std::println("Day {}:   {:>20}{:>20}{}", day + 3, std::move(part1),
               std::move(part2), stats);
  return stats;
}

#define SOLVE_DAY(Day)                                                         \
  solve_day<&Day##_parse, &Day##_part1, &Day##_part2>(#Day,                    \
                                                      "inputs/" #Day ".txt")
