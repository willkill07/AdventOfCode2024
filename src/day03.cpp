module;

#include <algorithm>
#include <ranges>
#include <string_view>

#include <ctre.hpp>

export module day03;

export using Day03ParsedType = std::string_view;
export using Day03AnswerType = long;

namespace stdv = std::views;

export Day03ParsedType Day03Parse(std::string_view input) noexcept {
  return input;
}

constexpr auto DoMultiply = [](auto&& match) -> long {
  auto [_, a, b] = match;
  return a.template to_number<long>() * b.template to_number<long>();
};

constexpr auto GetMuls = [](auto&& range) {
  return ctre::search_all<R"(mul\((\d+),(\d+)\))">(range) | stdv::transform(DoMultiply);
};

export Day03AnswerType Day03Part1(Day03ParsedType const& data) {
  return std::ranges::fold_left(GetMuls(data), 0L, std::plus{});
}

constexpr auto OnlyValid = [](auto&& x) { return ctre::split<R"(don't\(\))">(x) | stdv::take(1); };

constexpr auto FilterValid = [](std::string_view data) {
  return ctre::split<R"(do\(\))">(data) | stdv::transform(OnlyValid) | stdv::join;
};

export Day03AnswerType Day03Part2(Day03ParsedType const& data,
                                  [[maybe_unused]] Day03AnswerType const& answer) {
  return std::ranges::fold_left(FilterValid(data) | stdv::transform(GetMuls) | stdv::join, 0L, std::plus{});
}
