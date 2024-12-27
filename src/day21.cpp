
module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>

#include <ctre.hpp>

#include <print>

export module day21;

constexpr static std::size_t N{16};

constexpr static std::array lut3{
    0LU, 0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,
    0LU, 1LU,  21LU, 26LU, 16LU, 18LU, 18LU, 26LU, 21LU, 12LU, 27LU, 22LU, 13LU, 28LU, 23LU, 14LU,
    0LU, 19LU, 1LU,  18LU, 10LU, 0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,
    0LU, 20LU, 10LU, 1LU,  0LU,  19LU, 0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,
    0LU, 12LU, 18LU, 0LU,  1LU,  21LU, 0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,
    0LU, 10LU, 0LU,  25LU, 17LU, 1LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,  0LU,
    0LU, 10LU, 0LU,  0LU,  0LU,  0LU,  1LU,  25LU, 12LU, 19LU, 26LU, 13LU, 20LU, 27LU, 14LU, 21LU,
    0LU, 22LU, 0LU,  0LU,  0LU,  0LU,  21LU, 1LU,  10LU, 11LU, 12LU, 19LU, 20LU, 13LU, 20LU, 21LU,
    0LU, 17LU, 0LU,  0LU,  0LU,  0LU,  16LU, 18LU, 1LU,  10LU, 21LU, 12LU, 19LU, 22LU, 13LU, 20LU,
    0LU, 16LU, 0LU,  0LU,  0LU,  0LU,  21LU, 19LU, 18LU, 1LU,  22LU, 21LU, 12LU, 23LU, 22LU, 13LU,
    0LU, 23LU, 0LU,  0LU,  0LU,  0LU,  22LU, 16LU, 17LU, 18LU, 1LU,  10LU, 11LU, 12LU, 19LU, 20LU,
    0LU, 18LU, 0LU,  0LU,  0LU,  0LU,  17LU, 21LU, 16LU, 17LU, 18LU, 1LU,  10LU, 21LU, 12LU, 19LU,
    0LU, 17LU, 0LU,  0LU,  0LU,  0LU,  22LU, 22LU, 21LU, 16LU, 19LU, 18LU, 1LU,  22LU, 21LU, 12LU,
    0LU, 24LU, 0LU,  0LU,  0LU,  0LU,  23LU, 17LU, 18LU, 19LU, 16LU, 17LU, 18LU, 1LU,  10LU, 11LU,
    0LU, 19LU, 0LU,  0LU,  0LU,  0LU,  18LU, 22LU, 17LU, 18LU, 21LU, 16LU, 17LU, 18LU, 1LU,  10LU,
    0LU, 18LU, 0LU,  0LU,  0LU,  0LU,  23LU, 23LU, 22LU, 17LU, 22LU, 21LU, 16LU, 19LU, 18LU, 1LU};

constexpr static std::array lut26{0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           1LU,           27622800565LU, 30331287706LU,
                                  20790420654LU, 22411052532LU, 22411052532LU, 31420065370LU,
                                  28154654777LU, 14752615084LU, 31420065371LU, 28154654778LU,
                                  14752615085LU, 31420065372LU, 28154654779LU, 14752615086LU,
                                  0LU,           24095973437LU, 1LU,           22411052532LU,
                                  14287938116LU, 0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           25419021194LU, 14287938116LU, 1LU,
                                  0LU,           25419021193LU, 0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           14752615084LU, 22411052532LU, 0LU,
                                  1LU,           28154654777LU, 0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           14287938116LU, 0LU,           30331287705LU,
                                  22778092491LU, 1LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           0LU,           0LU,           0LU,
                                  0LU,           14287938116LU, 0LU,           0LU,
                                  0LU,           0LU,           1LU,           31420065369LU,
                                  14752615084LU, 24095973437LU, 31420065370LU, 14752615085LU,
                                  24095973438LU, 31420065371LU, 14752615086LU, 24095973439LU,
                                  0LU,           27052881364LU, 0LU,           0LU,
                                  0LU,           0LU,           27052881363LU, 1LU,
                                  14287938116LU, 14287938117LU, 14752615084LU, 24095973437LU,
                                  24095973438LU, 14752615085LU, 24095973438LU, 24095973439LU,
                                  0LU,           22778092491LU, 0LU,           0LU,
                                  0LU,           0LU,           20790420654LU, 22411052532LU,
                                  1LU,           14287938116LU, 28154654777LU, 14752615084LU,
                                  24095973437LU, 28154654778LU, 14752615085LU, 24095973438LU,
                                  0LU,           20790420654LU, 0LU,           0LU,
                                  0LU,           0LU,           27622800565LU, 22411052533LU,
                                  22411052532LU, 1LU,           28154654778LU, 28154654777LU,
                                  14752615084LU, 28154654779LU, 28154654778LU, 14752615085LU,
                                  0LU,           27052881365LU, 0LU,           0LU,
                                  0LU,           0LU,           27052881364LU, 20790420654LU,
                                  22778092491LU, 22778092492LU, 1LU,           14287938116LU,
                                  14287938117LU, 14752615084LU, 24095973437LU, 24095973438LU,
                                  0LU,           22778092492LU, 0LU,           0LU,
                                  0LU,           0LU,           20790420655LU, 27622800565LU,
                                  20790420654LU, 22778092491LU, 22411052532LU, 1LU,
                                  14287938116LU, 28154654777LU, 14752615084LU, 24095973437LU,
                                  0LU,           20790420655LU, 0LU,           0LU,
                                  0LU,           0LU,           27622800566LU, 27622800566LU,
                                  27622800565LU, 20790420654LU, 22411052533LU, 22411052532LU,
                                  1LU,           28154654778LU, 28154654777LU, 14752615084LU,
                                  0LU,           27052881366LU, 0LU,           0LU,
                                  0LU,           0LU,           27052881365LU, 20790420655LU,
                                  22778092492LU, 22778092493LU, 20790420654LU, 22778092491LU,
                                  22778092492LU, 1LU,           14287938116LU, 14287938117LU,
                                  0LU,           22778092493LU, 0LU,           0LU,
                                  0LU,           0LU,           20790420656LU, 27622800566LU,
                                  20790420655LU, 22778092492LU, 27622800565LU, 20790420654LU,
                                  22778092491LU, 22411052532LU, 1LU,           14287938116LU,
                                  0LU,           20790420656LU, 0LU,           0LU,
                                  0LU,           0LU,           27622800567LU, 27622800567LU,
                                  27622800566LU, 20790420655LU, 27622800566LU, 27622800565LU,
                                  20790420654LU, 22411052533LU, 22411052532LU, 1LU};

[[nodiscard]] constexpr unsigned Encode(char c) noexcept {
  switch (c) { // clang-format off
  case 'A': return 1;
  case 'v': return 2;
  case '<': return 3;
  case '>': return 4;
  case '^': return 5;
  default: return static_cast<unsigned>(c - '0') + 6U;
  } // clang-format on
}

export using Day21ParsedType = std::array<std::size_t, N * N>;
export using Day21AnswerType = std::size_t;

export Day21ParsedType Day21Parse(std::string_view input) noexcept {
  std::array<std::size_t, N * N> counts;
  counts.fill(0);
  input.remove_suffix(1);
  for (auto line : ctre::split<"\n">(input)) {
    auto num_str = line.view();
    num_str.remove_suffix(1);
    std::size_t num;
    std::from_chars(num_str.begin(), num_str.end(), num);
    for (unsigned last{Encode('A')}; unsigned curr : std::views::transform(line, Encode)) {
      counts[last * N + curr] += num;
      last = curr;
    }
  }
  return counts;
}

export Day21AnswerType Day21Part1(Day21ParsedType const& counts) noexcept {
  return std::ranges::fold_left(std::views::zip_transform(std::multiplies{}, counts, lut3), 0LU, std::plus{});
}

export Day21AnswerType Day21Part2(Day21ParsedType const& counts,
                                  [[maybe_unused]] Day21AnswerType const& answer) {
  return std::ranges::fold_left(
      std::views::zip_transform(std::multiplies{}, counts, lut26), 0LU, std::plus{});
}
