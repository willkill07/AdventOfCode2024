#include "day.hpp"
#include "timing.hpp"

#include <algorithm>
#include <charconv>
#include <iterator>
#include <print>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

constexpr static inline auto absdiff =
    []<typename T>(T const &x, std::same_as<T> auto const &y) noexcept {
      T const diff{x - y};
      return (diff < 0) ? -diff : diff;
    };

using day01_parsed = std::vector<std::pair<long, long>>;
using day01_answer = long;

[[nodiscard]] [[gnu::noinline]] static day01_parsed
day01_parse(std::span<char const> bytes) noexcept {
  day01_parsed res;
  res.reserve(1000);
  std::from_chars_result r{.ptr = bytes.data(), .ec = std::errc{}};
  char const *end{r.ptr + bytes.size_bytes()};
  while (r.ptr != end) {
    long f{0}, s{0};
    // parse the first number
    if (r = std::from_chars(r.ptr, end, f); r.ec != std::errc{})
      break;
    // consume all whitespace
    while (*++r.ptr == ' ')
      ;
    // parse the second number
    if (r = std::from_chars(r.ptr, end, s); r.ec != std::errc{})
      break;
    res.emplace_back(f, s);
    // increment consumes the newline character
    ++r.ptr;
  }
  std::ranges::sort(std::views::keys(res));
  std::ranges::sort(std::views::values(res));
  return res;
}

[[nodiscard]] [[gnu::noinline]] static day01_answer
day01_part1(day01_parsed const &data) noexcept {
  return std::ranges::fold_left(
      std::views::zip_transform(absdiff, std::views::keys(data),
                                std::views::values(data)),
      0l, std::plus{});
}

[[nodiscard]] [[gnu::noinline]] static auto
day01_part2(day01_parsed const &data,
            [[maybe_unused]] day01_answer const &answer) {
  std::ranges::random_access_range auto a = std::views::keys(data);
  std::ranges::random_access_range auto b = std::views::values(data);
  std::random_access_iterator auto ib = b.begin();
  auto compute = [&](long x) {
    // advance b until it is strictly >= x
    while (ib != b.end() and *ib < x) {
      ++ib;
    }
    std::random_access_iterator auto ob = ib;
    while (ib != b.end() and *ib == x) {
      ++ib;
    }
    return x * std::ranges::distance(ob, ib);
  };
  return std::ranges::fold_left(std::views::transform(a, compute), 0l,
                                std::plus{});
}

int main() {
  timing_stats stats;

  std::println("          {:>20s}{:>20s}{:>14s}{:>14s}{:>14s}{:>14s}", "Part 1",
               "Part 2", "Parse", "Part 1", "Part 2", "Total");
  std::println("          {:>20s}{:>20s}{:>14s}{:>14s}{:>14s}{:>14s}",
               "======", "======", "=====", "======", "======", "=====");

  // entrypoints go here for each day
  stats += SOLVE_DAY(day01);

  std::println("{:<10s}{:>20}{:>20}{}", "Total:", " --- ", " --- ", stats);
}
