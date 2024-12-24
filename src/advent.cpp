#include <charconv>
#include <chrono>
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <vector>

#include <cstdio>
#include <unistd.h>

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
import day14;
import day15;
import day16;
import day17;
import day18;
import day19;
import day20;
import day21;
import day22;
import day23;
import spinner;
import threading;
import util;

constexpr auto GetTimeAndUnits = [](long num) -> std::pair<long, std::string_view> {
  unsigned offset{0};
  while (num >= 10'000) {
    num /= 1'000;
    ++offset;
  }
  return std::pair{num, std::array{"ns", "µs", "ms", " s", "Ks", "Ms"}[offset]};
};

struct TimingStats {
  static inline int repetitions{1};

  long io{0}, parse{0}, part1{0}, part2{0}, total{0};

  constexpr TimingStats() noexcept = default;

  TimingStats(long io, long parse, long part1, long part2) noexcept
      : io{io}, parse{parse}, part1{part1}, part2{part2}, total{(io + parse + part1 + part2) / repetitions} {
  }

  constexpr inline TimingStats& operator+=(TimingStats const& other) noexcept {
    io += other.io;
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

static bool mask{false};

template <> struct std::formatter<TimingStats> {
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr inline auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE(readability-identifier-naming)
  inline auto format(TimingStats const& obj, std::format_context& ctx) const {

    auto [io, iol] = GetTimeAndUnits(obj.io / TimingStats::repetitions);
    auto [p, pl] = GetTimeAndUnits(obj.parse / TimingStats::repetitions);
    auto [p1, p1l] = GetTimeAndUnits(obj.part1 / TimingStats::repetitions);
    auto [p2, p2l] = GetTimeAndUnits(obj.part2 / TimingStats::repetitions);
    auto [t, tl] = GetTimeAndUnits(obj.total);
    // clang-format off
    return std::format_to(ctx.out(), "{:>4}{:2s} │ {:>4}{:2s} │ {:>4}{:2s} │ {:>4}{:2s} │ {:>4}{:2s}",
                          io, iol, p, pl, p1, p1l, p2, p2l, t, tl);
    // clang-format on
  }
};

[[nodiscard]] constexpr std::string_view Emoji(std::size_t num) {
  using std::string_view_literals::operator""sv;
  constexpr std::array const e{""sv,   "🍪"sv, "👺"sv, "🎨"sv, "🎥"sv, "🔥"sv, "🐱"sv, "🧮"sv,
                               "💨"sv, "💥"sv, "🤩"sv, "🐠"sv, "💻"sv, "🕹️"sv, "🤖"sv, "📦"sv,
                               "🦌"sv, "🖥️"sv, "🧱"sv, "🧻"sv, "🏎️"sv, "🔢"sv, "🍌"sv, "🛜"sv};
  if (num >= e.size()) {
    return "??";
  } else {
    return e[static_cast<unsigned>(num)];
  }
}

template <auto ParseFn, auto Part1Fn, auto Part2Fn>
[[nodiscard]] [[gnu::noinline]] static TimingStats SolveDay(Spinner& spinner, unsigned day_num) noexcept {

  std::string const filename{std::format("inputs/Day{:02d}.txt", day_num)};
  using ClockType = std::chrono::steady_clock;

  if (spinner.HasTTY()) {
    std::print("│  {0:02d} │ {1:17s} │ {1:38s} │ {1:6s} │ {1:6s} │ {1:6s} │ {1:6s} │ {1:6s} │ {2:2s} │",
               day_num,
               "",
               Emoji(day_num));
    std::cout.flush();
    spinner.Enable();
    spinner.SetLocation(TimeType::File);
  }

  // File IO
  ClockType::time_point const t0 = ClockType::now();
  std::string input{util::ReadFile(filename.c_str())};
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = util::ReadFile(filename.c_str());
  }
  ClockType::time_point t1 = ClockType::now();
  auto const io_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  if (spinner.HasTTY()) {
    auto [time, units] = GetTimeAndUnits(io_time / TimingStats::repetitions);
    spinner.PutTime(TimeType::File, time, units);
    spinner.SetLocation(TimeType::Parse);
  }

  // Parsing
  t1 = ClockType::now();
  auto data = ParseFn(input);
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = ParseFn(input);
  }
  ClockType::time_point t2 = ClockType::now();
  auto const parse_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
  if (spinner.HasTTY()) {
    auto [time, units] = GetTimeAndUnits(parse_time / TimingStats::repetitions);
    spinner.PutTime(TimeType::Parse, time, units);
    spinner.SetLocation(TimeType::Part1);
  }

  // Part 1
  t2 = ClockType::now();
  auto part1 = Part1Fn(data);
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = Part1Fn(data);
  }
  ClockType::time_point t3 = ClockType::now();
  auto const part1_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count();
  auto p1 = [&] {
    if constexpr (std::is_constructible_v<std::string, decltype(part1)>) {
      return part1;
    } else {
      return std::to_string(part1);
    }
  }();
  if (mask) {
    std::ranges::fill(p1, 'X');
  }
  if (spinner.HasTTY()) {
    auto [time, units] = GetTimeAndUnits(part1_time / TimingStats::repetitions);
    spinner.PutTime(TimeType::Part1, time, units);
    spinner.PutAnswer(AnswerType::Part1, p1);
    spinner.SetLocation(TimeType::Part2);
  }

  // Part 2
  t3 = ClockType::now();
  auto const part2 = Part2Fn(data, part1);
  for (int i = 1; i < TimingStats::repetitions; ++i) {
    std::ignore = Part2Fn(data, part1);
  }
  ClockType::time_point const t4 = ClockType::now();
  auto const part2_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
  auto p2 = [&] {
    if constexpr (std::is_constructible_v<std::string, decltype(part2)>) {
      return part2;
    } else {
      return std::to_string(part2);
    }
  }();
  if (mask) {
    std::ranges::fill(p2, 'X');
  }

  TimingStats const stats{io_time, parse_time, part1_time, part2_time};
  if (spinner.HasTTY()) {
    auto [time, units] = GetTimeAndUnits(part2_time / TimingStats::repetitions);
    spinner.PutTime(TimeType::Part2, time, units);
    spinner.PutAnswer(AnswerType::Part2, p2);
    auto [total_time, total_units] = GetTimeAndUnits(stats.total);
    spinner.PutTime(TimeType::Total, total_time, total_units);
    spinner.Disable();
    spinner.Sync();
    std::println("");
    std::cout.flush();
  } else {
    std::println("│  {:02d} │ {: <17} │ {: <37} │ {} │ {:s} │",
                 day_num,
                 std::move(p1),
                 std::move(p2),
                 stats,
                 Emoji(day_num));
  }
  return stats;
}

constexpr std::array DAYS{
    SolveDay<&Day01Parse, &Day01Part1, &Day01Part2>, SolveDay<&Day02Parse, &Day02Part1, &Day02Part2>,
    SolveDay<&Day03Parse, &Day03Part1, &Day03Part2>, SolveDay<&Day04Parse, &Day04Part1, &Day04Part2>,
    SolveDay<&Day05Parse, &Day05Part1, &Day05Part2>, SolveDay<&Day06Parse, &Day06Part1, &Day06Part2>,
    SolveDay<&Day07Parse, &Day07Part1, &Day07Part2>, SolveDay<&Day08Parse, &Day08Part1, &Day08Part2>,
    SolveDay<&Day09Parse, &Day09Part1, &Day09Part2>, SolveDay<&Day10Parse, &Day10Part1, &Day10Part2>,
    SolveDay<&Day11Parse, &Day11Part1, &Day11Part2>, SolveDay<&Day12Parse, &Day12Part1, &Day12Part2>,
    SolveDay<&Day13Parse, &Day13Part1, &Day13Part2>, SolveDay<&Day14Parse, &Day14Part1, &Day14Part2>,
    SolveDay<&Day15Parse, &Day15Part1, &Day15Part2>, SolveDay<&Day16Parse, &Day16Part1, &Day16Part2>,
    SolveDay<&Day17Parse, &Day17Part1, &Day17Part2>, SolveDay<&Day18Parse, &Day18Part1, &Day18Part2>,
    SolveDay<&Day19Parse, &Day19Part1, &Day19Part2>, SolveDay<&Day20Parse, &Day20Part1, &Day20Part2>,
    SolveDay<&Day21Parse, &Day21Part1, &Day21Part2>, SolveDay<&Day22Parse, &Day22Part1, &Day22Part2>,
    SolveDay<&Day23Parse, &Day23Part1, &Day23Part2>};

int main(int argc, char* argv[]) {
  bool const has_tty{static_cast<bool>(::isatty(STDOUT_FILENO))};
  threading::Initialize();
  Spinner spinner{has_tty};
  std::vector<std::string_view> args{argv, argv + argc};
  if (args.size() > 1) {
    std::ignore = std::from_chars(args[1].begin(), args[1].end(), TimingStats::repetitions);
    if (TimingStats::repetitions > 1) {
      std::println("Note: doing {} repetitions of each.", TimingStats::repetitions);
    }
  }
  if (args.size() > 2) {
    mask = (args[2] == "mask");
    if (mask) {
      std::println("Note: masking answers.");
    }
  }

  TimingStats stats;
  // clang-format off
  std::println("      ╭────────────────────────────────────────────────────────────┬────────────────────────────────────────────╮");
  std::println("      │                          Answers                           │                   Timing                   │");
  std::println("╭─────┼───────────────────┬────────────────────────────────────────┼────────┬────────┬────────┬────────┬────────┼────╮");
  std::println("│ Day │ Part 1            │ Part 2                                 │  I/O   │ Parse  │ Part 1 │ Part 2 │ Total  │ 🏆 │");
  std::println("├─────┼───────────────────┼────────────────────────────────────────┼────────┼────────┼────────┼────────┼────────┼────┤");
  for (auto&& [day, fn] : std::views::zip(std::views::iota(1U), DAYS)) {
    stats += fn(spinner, day);
  }
  std::println("╰─────┼───────────────────┴────────────────────────────────────────┼────────┼────────┼────────┼────────┼────────┼────╯");
  std::println("      │ AoC++ 2024 in C++23 by willkill07                          │ {} │", stats);
  std::println("      ╰────────────────────────────────────────────────────────────┴────────┴────────┴────────┴────────┴────────╯");
  // clang-format on
}
