module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

export module day25;

import threading;

export using Day25ParsedType = std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>;
export using Day25AnswerType = long;

export Day25ParsedType Day25Parse(std::string_view input) noexcept {
  Day25ParsedType parsed;
  for (auto const& grid : ctre::split<"\n\n">(input)) {
    std::size_t const value{
        std::ranges::fold_left(std::views::transform(grid, std::bind_front(std::equal_to{}, '#')),
                               0U,
                               [](std::size_t value, bool b) { return (value << 1) | b; })};
    (grid.data()[0] == '#' ? std::get<0>(parsed) : std::get<1>(parsed)).push_back(value);
  }
  return parsed;
}

export long Day25Part1(Day25ParsedType const& data) noexcept {
  return threading::ParallelReduceAdd(std::get<0>(data), [&] (std::size_t lock) {
    return std::ranges::count_if(std::get<1>(data), [&] (std::size_t key) {
      return (lock & key) == 0ZU;
    });
  }, 0L, /*threads=*/4);
}

export std::string Day25Part2([[maybe_unused]] Day25ParsedType const& data,
                              [[maybe_unused]] Day25AnswerType const& answer) {
  return "Happy Advent of Code 2024!";
}
