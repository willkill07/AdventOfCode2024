module;

#include <algorithm>
#include <ranges>
#include <string_view>

#include <ctre.hpp>
#include <ctre/range.hpp>

export module day03;

export using Day03ParsedType = std::string_view;
export using Day03AnswerType = long;

export Day03ParsedType Day03Parse(std::string_view input) noexcept {
  return input;
}

constexpr auto DoMultiply = [](auto&& match) {
  long a{0}, b{0};
  auto ra = match.template get<"a">();
  auto rb = match.template get<"b">();
  std::from_chars(ra.begin(), ra.end(), a);
  std::from_chars(rb.begin(), rb.end(), b);
  return a * b;
};

constexpr auto FindAllMuls = [](auto range) {
  return std::views::transform(ctre::search_all<R"(mul\((?<a>\d+),(?<b>\d+)\))">(range),
                               [](auto&& x) { return DoMultiply(x); });
};

export Day03AnswerType Day03Part1(Day03ParsedType const& data) {
  return std::ranges::fold_left(FindAllMuls(data), 0L, std::plus{});
}

export Day03AnswerType Day03Part2(Day03ParsedType const& data,
                                  [[maybe_unused]] Day03AnswerType const& answer) {

  return std::ranges::fold_left(
      std::views::join(std::views::transform(
          std::views::join(std::views::transform(
              ctre::split<"do\\(\\)">(data),
              [](auto&& v) { return ctre::split<"don't\\(\\)">(v) | std::views::take(1); })),
          FindAllMuls)),
      0L,
      std::plus{});
}
