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

constexpr auto Rule = [](std::ranges::random_access_range auto&& array, int a, int b) -> decltype(auto) {
  return array[static_cast<std::uint32_t>(a * Size + b)];
};

export Day05ParsedType Day05Parse(std::string_view input) noexcept {
  Day05ParsedType parsed;
  std::size_t const split{input.find("\n\n")};
  parsed.rules.fill(false);
  for (auto match : ctre::search_all<R"((\d\d)\|(\d\d))">(input.substr(0, split))) {
    auto [_, a, b] = match;
    Rule(parsed.rules, a.to_number() - Lower, b.to_number() - Lower) = true;
  }
  parsed.updates = ctre::split<"\n">(input.substr(split + 2)) |
                   std::views::filter([](auto&& line) { return line.size() > 0; }) |
                   std::views::transform([]<typename Line>(Line&& line) {
                     return ctre::search_all<R"(\d\d)">(std::forward<Line>(line)) |
                            std::views::transform([](auto&& num) { return num.to_number() - Lower; }) |
                            std::ranges::to<Update>();
                   }) |
                   std::ranges::to<std::vector>();
  return parsed;
}

constexpr auto Comparator = [](Rules const& rules) {
  return [&rules](int a, int b) { return Rule(rules, a, b); };
};

constexpr auto CheckSorted = []<typename Comp>(Comp&& comparator, std::vector<int> const& update) {
  return std::ranges::is_sorted(update, std::forward<Comp>(comparator));
};

constexpr auto GetMidpoint = [](std::vector<int> const& range) { return Lower + range[range.size() / 2]; };

export Day05AnswerType Day05Part1(Day05ParsedType const& data) {
  auto check = std::bind_front(CheckSorted, Comparator(data.rules));
  return std::ranges::fold_left(data.updates | std::views::filter(std::move(check)) |
                                    std::views::transform(GetMidpoint),
                                0,
                                std::plus{});
}

constexpr auto SortAndGetMidpoint = [](auto&& comparator, std::vector<int> update) {
  auto midpoint = update.begin() + static_cast<long>(update.size() / 2);
  std::ranges::nth_element(update, midpoint, comparator);
  return GetMidpoint(update);
};

export Day05AnswerType Day05Part2(Day05ParsedType const& data,
                                  [[maybe_unused]] Day05AnswerType const& answer) {
  auto check = std::not_fn(std::bind_front(CheckSorted, Comparator(data.rules)));
  auto action = std::bind_front(SortAndGetMidpoint, Comparator(data.rules));
  return std::ranges::fold_left(data.updates | std::views::filter(std::move(check)) |
                                    std::views::transform(std::move(action)),
                                0,
                                std::plus{});
}
