module;

#include <algorithm>
#include <concepts>
#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

export module day01;

constexpr auto AbsDiff = []<typename T>(T const& x, std::same_as<T> auto const& y) noexcept {
  return x > y ? x - y : y - x;
};

export using Day01ParsedType = std::vector<std::pair<long, long>>;
export using Day01AnswerType = long;

export Day01ParsedType Day01Parse(std::string_view input) noexcept {
  auto res = ctre::search_all<R"((\d+) +(\d+))">(input) | std::views::transform([](auto x) {
               auto [_, a, b] = x;
               return std::pair{a.template to_number<long>(), b.template to_number<long>()};
             }) |
             std::ranges::to<std::vector>();
  std::ranges::sort(std::views::keys(res));
  std::ranges::sort(std::views::values(res));
  return res;
}

export Day01AnswerType Day01Part1(Day01ParsedType const& data) noexcept {
  return std::ranges::fold_left(
      std::views::zip_transform(AbsDiff, std::views::keys(data), std::views::values(data)), 0L, std::plus{});
}

export Day01AnswerType Day01Part2(Day01ParsedType const& data,
                                  [[maybe_unused]] Day01AnswerType const& answer) {
  auto a = std::views::keys(data);
  auto b = std::views::values(data);
  // clang-format off
  auto reduce = [end = b.end()](auto&& pair, long x) {
    auto [acc, i] = pair;
    auto j = i; while (j != end and x > *j) { ++j; }
    auto k = j; while (k != end and x == *k) { ++k; }
    return std::pair{acc + x * std::distance(j, k), i + std::distance(i, k)};
  };
  // clang-format on
  return std::ranges::fold_left(a, std::pair{0L, b.begin()}, std::move(reduce)).first;
}
