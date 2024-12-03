#include "day.hpp"
#include "timing.hpp"

#include <algorithm>
#include <charconv>
#include <functional>
#include <iterator>
#include <print>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

constexpr static inline auto absdiff = []<typename T>(T const& x, std::same_as<T> auto const& y) noexcept {
  T const diff{x - y};
  return (diff < 0) ? -diff : diff;
};

using Day01ParsedType = std::vector<std::pair<long, long>>;
using Day01AnswerType = long;

[[nodiscard]] [[gnu::noinline]] static Day01ParsedType Day01Parse(std::string_view input) noexcept {
  Day01ParsedType res;
  res.reserve(1000);
  std::from_chars_result r{.ptr = input.data(), .ec = std::errc{}};
  char const* end{r.ptr + input.size()};
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

using Day02ParsedType = std::vector<std::vector<long>>;
using Day02AnswerType = long;

[[nodiscard]] [[gnu::noinline]] static Day02ParsedType Day02Parse(std::string_view input) noexcept {
  return input | std::views::split('\n') | std::views::filter([](auto&& line) { return not line.empty(); }) |
         std::views::transform([](auto&& line) {
           return line | std::views::split(' ') | std::views::transform([](auto&& num) -> long {
                    long v{0};
                    std::ignore = std::from_chars(num.begin(), num.end(), v);
                    return v;
                  }) |
                  std::ranges::to<std::vector>();
         }) |
         std::ranges::to<std::vector>();
}

[[nodiscard]] static bool Day02CheckLevel(std::vector<long> const& level) {
  std::ranges::random_access_range auto diffs = std::views::adjacent_transform<2>(level, std::minus{});
  return std::ranges::fold_left(diffs, (diffs.front() != 0), [pos{diffs.front() > 0}](bool valid, long diff) {
    return valid and (pos ? (1 <= diff and diff <= 3) : (-3 <= diff and diff <= -1));
  });
}

[[nodiscard]] static bool Day02CheckLevelTolerant(std::vector<long>& temp, std::vector<long> const& level) {
  // populate with all-but-first value
  temp.assign(level.begin() + 1, level.end());
  return std::ranges::any_of(std::views::iota(0, static_cast<int>(level.size())), [&](int i) {
    if (i > 0) {
      // only need to reassign a single element (replace new removed value with the one removed prior)
      *std::next(temp.begin(), i - 1) = *std::next(level.begin(), i - 1);
    }
    return Day02CheckLevel(temp);
  });
}

[[nodiscard]] [[gnu::noinline]] static Day02AnswerType Day02Part1(Day02ParsedType const& data) noexcept {
  return std::ranges::count_if(data, Day02CheckLevel);
}

[[nodiscard]] [[gnu::noinline]] static Day02AnswerType
Day02Part2(Day02ParsedType const& data, [[maybe_unused]] Day02AnswerType const& answer) {
  // let's at least pretend we care about performance by reducing (re)allocations
  static std::vector<long> curr;
  return std::ranges::count_if(data, std::bind_front(Day02CheckLevelTolerant, curr));
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
  stats += SOLVE_DAY(Day02);

  std::println("{:<10s}{:>20}{:>20}{}", "Total:", " --- ", " --- ", stats);
}
