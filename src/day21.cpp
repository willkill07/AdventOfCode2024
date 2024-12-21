module;

#include <string_view>

#include <ctre.hpp>

export module day21;

export using Day21ParsedType = std::string_view;
export using Day21AnswerType = long;

export Day21ParsedType Day21Parse(std::string_view input) noexcept {
  return input;
}

export Day21AnswerType Day21Part1(Day21ParsedType const& data) noexcept {
  return 0;
}

export Day21AnswerType Day21Part2(Day21ParsedType const& data,
                                  [[maybe_unused]] Day21AnswerType const& answer) {
  return 0;
}
