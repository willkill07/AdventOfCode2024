#include "day.hpp"

#include <algorithm>
#include <charconv>
#include <iterator>
#include <ranges>

namespace day01 {
using parsed = std::pair<std::vector<int>, std::vector<int>>;
using part1_answer = int;
using part2_answer = long;

[[nodiscard]] [[gnu::noinline]] static auto
do_parse(std::span<char const> bytes) {
  std::pair<std::vector<int>, std::vector<int>> pair;
  pair.first.reserve(1000);
  pair.second.reserve(1000);
  auto lines = bytes | std::views::split('\n');
  for (auto l : lines) {
    if (l.empty()) {
      break;
    }
    auto f1 = l.begin();
    auto f2 = std::ranges::find(l, ' ');
    auto s1 = std::ranges::find(std::views::reverse(l), ' ').base();
    auto s2 = l.end();
    int f{0}, s{0};
    std::from_chars(f1.base(), f2.base(), f);
    std::from_chars(s1.base(), s2.base(), s);
    pair.first.emplace_back(f);
    pair.second.emplace_back(s);
  }
  std::ranges::sort(pair.first);
  std::ranges::sort(pair.second);
  return pair;
}

[[nodiscard]] [[gnu::noinline]] static part1_answer
do_part1(parsed const &data) {
  return std::ranges::fold_left(
      std::views::zip_transform([](int a, int b) { return std::abs(a - b); },
                                data.first, data.second),
      0, std::plus{});
}

[[nodiscard]] [[gnu::noinline]] static part2_answer
do_part2(parsed const &data, [[maybe_unused]] part1_answer const &answer) {
  auto const end = data.second.end();
  auto curr = data.second.begin();
  return std::ranges::fold_left(
      std::views::transform(data.first,
                            [&](int f) {
                              // advance second to when they are strictly >=
                              while (curr != end and *curr < f) {
                                ++curr;
                              }
                              auto old = curr;
                              while (curr != end and *curr == f) {
                                ++curr;
                              }
                              return f * std::distance(old, curr);
                            }),
      0, std::plus{});
}

} // namespace day01

int main() { SOLVE_DAY(day01); }
