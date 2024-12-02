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

constexpr static inline auto absdiff = []<typename T>(T const& x, std::same_as<T> auto const& y) noexcept {
  T const diff{x - y};
  return (diff < 0) ? -diff : diff;
};

using Day01ParsedType = std::vector<std::pair<long, long>>;
using Day01AnswerType = long;

[[nodiscard]] [[gnu::noinline]] static Day01ParsedType Day01Parse(std::span<char const> bytes) noexcept {
  Day01ParsedType res;
  res.reserve(1000);
  std::from_chars_result r{.ptr = bytes.data(), .ec = std::errc{}};
  char const* end{r.ptr + bytes.size_bytes()};
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

[[nodiscard]] [[gnu::noinline]] static Day01AnswerType Day01Part1(Day01ParsedType const& data) noexcept {
  return std::ranges::fold_left(
      std::views::zip_transform(absdiff, std::views::keys(data), std::views::values(data)), 0L, std::plus{});
}

[[nodiscard]] [[gnu::noinline]] static auto Day01Part2(Day01ParsedType const& data,
                                                       [[maybe_unused]] Day01AnswerType const& answer) {
  std::ranges::random_access_range auto a = std::views::keys(data);
  std::ranges::random_access_range auto b = std::views::values(data);
  std::random_access_iterator auto ib = b.begin();
  auto compute = [&](long x) noexcept -> long {
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
  return std::ranges::fold_left(std::views::transform(a, compute), 0L, std::plus{});
}

int main(int argc, char* argv[]) {
  std::vector<std::string_view> args{argv, argv + argc};
  if (args.size() > 1) {
    std::ignore = std::from_chars(args[1].begin(), args[1].end(), TimingStats::repetitions);
    if (TimingStats::repetitions > 1) {
      std::println("Note: doing {} repetitions of each.", TimingStats::repetitions);
    }
  }

  TimingStats stats;

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

  // entrypoints go here for each day
  stats += SOLVE_DAY(Day01);

  std::println("{:<10s}{:>20}{:>20}{}", "Total:", " --- ", " --- ", stats);
}
