
module;

#include <algorithm>
#include <array>
#include <functional>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include <ctre.hpp>

export module day05;

// Model the entire ruleset as a big(?) lookup table -- density is around 14.5%
constexpr int Lower{10};
constexpr int Upper{99};
constexpr int Size{Upper - Lower + 1};
using Rules = std::array<bool, static_cast<std::uint32_t>(Size* Size)>;

using Update = std::vector<int>;

export struct Day05ParsedType {
  Rules rules;
  std::vector<Update> updates;
};
export using Day05AnswerType = int;

namespace stdv = std::views;

constexpr auto Rule = [](std::ranges::random_access_range auto&& array, int a, int b) -> decltype(auto) {
  return array[static_cast<std::uint32_t>(a * Size + b)];
};

export Day05ParsedType Day05Parse(std::string_view input) noexcept {
  Day05ParsedType parsed;
  std::size_t const split{input.find("\n\n")};
  parsed.rules.fill(false);
  for (auto match : ctre::search_all<R"((\d\d)\|(\d\d))">(input.substr(0, split))) {
    auto [_, a, b] = match;
    Rule(parsed.rules, b.to_number() - Lower, a.to_number() - Lower) = true;
  }
  parsed.updates = ctre::split<"\n">(input.substr(split + 2)) |
                   stdv::filter([](auto&& line) { return line.size() > 0; }) |
                   stdv::transform([](auto&& line) {
                     return ctre::search_all<R"(\d\d)">(line) |
                            stdv::transform([](auto&& num) { return num.to_number() - Lower; }) |
                            std::ranges::to<Update>();
                   }) |
                   std::ranges::to<std::vector>();
  return parsed;
}

constexpr auto CheckSorted = [](Rules const& rules, std::vector<int> const& update) {
  for (auto i = update.begin(); i != update.end(); ++i) {
    for (auto j = std::next(i); j != update.end(); ++j) {
      if (Rule(rules, *i, *j)) {
        return false;
      }
    }
  }
  return true;
};

constexpr auto GetMidpoint = [](auto&& range) { return Lower + range[range.size() / 2]; };

export Day05AnswerType Day05Part1(Day05ParsedType const& data) {
  return std::ranges::fold_left(
      stdv::transform(stdv::filter(data.updates, std::bind_front(CheckSorted, data.rules)), GetMidpoint),
      0,
      std::plus{});
  return 0;
}

constexpr auto SortAndGetMidpoint = [](Rules const& rules, std::vector<int> update) {
  bool swapped{false};
  for (auto i = update.begin(); i != update.end(); ++i) {
    for (auto j = std::next(i); j != update.end(); ++j) {
      if (Rule(rules, *i, *j)) {
        std::iter_swap(i, j);
        swapped = true;
      }
    }
  }
  return swapped ? GetMidpoint(update) : 0;
};

export Day05AnswerType Day05Part2(Day05ParsedType const& data,
                                  [[maybe_unused]] Day05AnswerType const& answer) {
  return std::ranges::fold_left(
      stdv::transform(data.updates, std::bind_front(SortAndGetMidpoint, data.rules)), 0, std::plus{});
}
