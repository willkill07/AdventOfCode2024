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
  long offset{0};
  std::ranges::random_access_range auto a = std::views::keys(data);
  std::ranges::random_access_range auto b = std::views::values(data);
  std::ranges::random_access_range auto check{b | std::views::drop(0)};
  
  auto compute = [&](long x) noexcept -> long {
    auto skip_range = check | std::views::take_while(std::bind_front(std::greater{}, x));
    long const skip{std::ranges::distance(skip_range)};
    auto dist_range = check | std::views::drop(skip) | std::views::take_while(std::bind_front(std::equal_to{}, x));
    long const dist{std::ranges::distance(dist_range)};
    offset += skip + dist;
    check = b | std::views::drop(offset);
    return x * dist;
  };
  return std::ranges::fold_left(std::views::transform(a, compute), 0L, std::plus{});
}
