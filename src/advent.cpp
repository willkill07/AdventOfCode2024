#include <charconv>
#include <chrono>
#include <format>
#include <print>
#include <vector>

import day01;
import day02;
import day03;
import util;

struct TimingStats {
  static inline int repetitions{1};

  long parse{0};
  long part1{0};
  long part2{0};
  long total{0};

  constexpr inline TimingStats& operator+=(TimingStats const& other) noexcept {
    parse += other.parse;
    part1 += other.part1;
    part2 += other.part2;
    total += other.total;
    return *this;
  }

  [[nodiscard]] constexpr inline TimingStats operator+(TimingStats const& other) noexcept {
    return TimingStats{*this} += other;
  }
};

template <> struct std::formatter<TimingStats> {
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr inline auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE(readability-identifier-naming)
  inline auto format(TimingStats const& obj, std::format_context& ctx) const {
    return std::format_to(ctx.out(),
                          "{:>12}µs{:>12}µs{:>12}µs{:>12}µs",
                          obj.parse / TimingStats::repetitions,
                          obj.part1 / TimingStats::repetitions,
                          obj.part2 / TimingStats::repetitions,
                          obj.total / TimingStats::repetitions);
  }
};

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

int main(int argc, char* argv[]) {
  std::vector<std::string_view> args{argv, argv + argc};
  if (args.size() > 1) {
    std::ignore = std::from_chars(args[1].begin(), args[1].end(), TimingStats::repetitions);
    if (TimingStats::repetitions > 1) {
      std::println("Note: doing {} repetitions of each.", TimingStats::repetitions);
    }
  }

  std::println("          {:>20s}{:>20s}{:>14s}{:>14s}{:>14s}{:>14s}",
               "Part 1",
               "Part 2",
               "Parse",
               "Part 1",
               "Part 2",
               "Total");
  std::println("          {:>20s}{:>20s}{:>14s}{:>14s}{:>14s}{:>14s}",
               "======",
               "======",
               "=====",
               "======",
               "======",
               "=====");

  TimingStats stats;
  // entrypoints go here for each day
  stats += SOLVE_DAY(Day01);
  stats += SOLVE_DAY(Day02);
  stats += SOLVE_DAY(Day03);

  std::println("{:<10s}{:>20}{:>20}{}", "Total:", " --- ", " --- ", stats);
}
