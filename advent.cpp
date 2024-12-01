#include "day.hpp"

#include <algorithm>
#include <charconv>
#include <iterator>
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

[[nodiscard]] [[gnu::noinline]] static auto
day01_parse(std::span<char const> bytes) {
  std::vector<std::pair<long, long>> res;
  res.reserve(1000);
  for (std::ranges::subrange line : std::views::split(bytes, '\n')) {
    if (line.empty()) {
      break;
    }
    int f{0}, s{0};
    std::random_access_iterator auto x = std::ranges::find(line, ' ');
    std::from_chars(line.begin().base(), x.base(), f);
    std::ranges::subrange y = std::ranges::find_last(line, ' ');
    std::from_chars(y.begin().base() + 1, y.end().base(), s);
    res.emplace_back(f, s);
  }
  std::ranges::sort(std::views::keys(res));
  std::ranges::sort(std::views::values(res));
  return res;
}

[[nodiscard]] [[gnu::noinline]] static auto
day01_part1(day01_parsed const &data) {
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
    // advance b until they are strictly >=
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
  // entrypoints go here for each day...
  SOLVE_DAY(day01);
}
