module;

#include <algorithm>
#include <concepts>
#include <functional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <vector>

#include <ctre.hpp>

export module day07;

template <bool B> constexpr static inline auto Part2 = std::bool_constant<B>{};

template <typename T>
concept Part2Flag = std::same_as<T, std::bool_constant<true>> or std::same_as<T, std::bool_constant<false>>;

struct Trial {
  long candidate;
  std::vector<long> sequence;

private:
  [[nodiscard]] bool CanReach(long target, auto iter, Part2Flag auto part2) const noexcept {
    if (long const curr{*--iter}; iter == sequence.begin()) {
      return target == curr;
    } else if (long diff = target - curr; diff >= 0 and CanReach(diff, iter, part2)) {
      return true;
    } else if (auto const [q, r] = std::div(target, curr); r == 0 and CanReach(q, iter, part2)) {
      return true;
    } else if constexpr (part2) {
      long num = 10;
      while (curr >= num) {
        num *= 10;
      }
      if (auto const [left, right] = std::div(target, num); right == curr and CanReach(left, iter, part2)) {
        return true;
      }
    }
    return false;
  }

public:
  [[nodiscard]] long Check(Part2Flag auto part2) const noexcept {
    return CanReach(candidate, sequence.end(), part2) ? candidate : 0;
  }
};

export using Day07ParsedType = std::vector<Trial>;
export using Day07AnswerType = long;

export Day07ParsedType Day07Parse(std::string_view input) noexcept {
  return ctre::split<"\n">(input) | std::views::filter([](auto&& line) { return line.size() != 0; }) |
         std::views::transform([](auto&& line) {
           auto colon = line.view().find(':');
           auto target = line.view().substr(0, colon);
           long candidate{0};
           std::from_chars(target.begin(), target.end(), candidate);
           return Trial{.candidate = candidate,
                        .sequence = ctre::split<" ">(line.view().substr(colon + 2)) |
                                    std::views::transform(
                                        [](auto&& match) { return match.template to_number<long>(); }) |
                                    std::ranges::to<std::vector>()};
         }) |
         std::ranges::to<std::vector>();
}

export Day07AnswerType Day07Part1(Day07ParsedType const& data) noexcept {
  return std::ranges::fold_left(
      std::views::transform(data, [](Trial const& t) { return t.Check(Part2<false>); }), 0L, std::plus{});
}

export Day07AnswerType Day07Part2(Day07ParsedType const& data,
                                  [[maybe_unused]] Day07AnswerType const& answer) {
  return std::ranges::fold_left(
      std::views::transform(data, [](Trial const& t) { return t.Check(Part2<true>); }), 0L, std::plus{});
}
