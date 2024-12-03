#pragma once

#include "timing.hpp"
#include "util.hpp"

#include <chrono>
#include <print>

template <auto ParseFn, auto Part1Fn, auto Part2Fn>
[[nodiscard]] [[gnu::noinline]] static TimingStats SolveDay(char const* day, char const* filename) noexcept {
  using ClockType = std::chrono::steady_clock;
  std::string const input{util::ReadFile(filename)};
  ClockType::time_point const t0 = ClockType::now();
  auto const data = ParseFn(input);
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = ParseFn(input);
  }
  ClockType::time_point const t1 = ClockType::now();
  auto const part1 = Part1Fn(data);
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = Part1Fn(data);
  }
  ClockType::time_point const t2 = ClockType::now();
  auto const part2 = Part2Fn(data, part1);
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = Part2Fn(data, part1);
  }
  ClockType::time_point const t3 = ClockType::now();
  TimingStats const stats{.parse = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count(),
                          .part1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count(),
                          .part2 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count(),
                          .total = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t0).count()};
  std::println("Day {}:   {:>20}{:>20}{}", day + 3, std::move(part1), std::move(part2), stats);
  return stats;
}

#define SOLVE_DAY(Day) SolveDay<&Day##Parse, &Day##Part1, &Day##Part2>(#Day, "inputs/" #Day ".txt")
