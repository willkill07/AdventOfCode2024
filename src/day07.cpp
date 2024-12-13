module;

#include <algorithm>
#include <charconv>
#include <concepts>
#include <functional>
#include <ranges>
#include <string_view>
#include <type_traits>
#include <vector>

#include <ctre.hpp>
#include <print>

export module day07;

template <bool B> constexpr static inline auto Part2 = std::bool_constant<B>{};

static constexpr auto Div = [] <std::integral T>(T a, std::same_as<T> auto b) {
    return std::pair{a / b, a % b};
};

template <typename T>
concept Part2Flag = std::same_as<T, std::bool_constant<true>> or std::same_as<T, std::bool_constant<false>>;

struct Trial {
  long candidate;
  std::vector<long> sequence;

private:
  [[nodiscard]] bool CanReach(long target, std::size_t iter, Part2Flag auto part2) const noexcept {
    if (long const curr{sequence[--iter]}; iter == 0) {
      return target == curr;
    } else if (long diff = target - curr; diff >= 0 and CanReach(diff, iter, part2)) {
      return true;
    } else if (auto const [q, r] = Div(target, curr); r == 0 and CanReach(q, iter, part2)) {
      return true;
    } else if constexpr (part2) {
      long num = 10;
      while (curr >= num) {
        num *= 10;
      }
      if (auto const [left, right] = Div(target, num); right == curr and CanReach(left, iter, part2)) {
        return true;
      }
    }
    return false;
  }

public:
  [[nodiscard]] long Check(Part2Flag auto part2) const noexcept {
    return CanReach(candidate, sequence.size(), part2) ? candidate : 0;
  }
};

export using Day07ParsedType = std::vector<Trial>;
export using Day07AnswerType = long;

export Day07ParsedType Day07Parse(std::string_view input) noexcept {
  std::vector<Trial> trials;
  trials.reserve(850);
  for (auto i = input.begin(); i != input.end(); ++i) {
    long target;
    auto [ii, _] = std::from_chars(i, input.end(), target);
    // advance past ': '
    i = ii + 2;
    std::vector<long> values;
    values.reserve(12);
    while (true) {
      long val;
      auto [iii, _] = std::from_chars(i, input.end(), val);
      values.push_back(val);
      i = iii;
      if (*i == '\n') {
        break;
      }
      // advance past ' ';
      ++i;
    }
    trials.emplace_back(target, std::move(values));
  }
  return trials;
}

long p2{0};

export Day07AnswerType Day07Part1(Day07ParsedType const& data) noexcept {
  long p1{0};
  p2 = 0;
  // it's faster to conditionally process part 2 with part 1
  for (auto const& t : data) {
    if (long v1{t.Check(Part2<false>)}; v1 > 0) {
      p1 += v1;
    } else if (long v2{t.Check(Part2<true>)}; v2 > 0) {
      p2 += v2;
    }
  }
  return p1;
}

export Day07AnswerType Day07Part2([[maybe_unused]] Day07ParsedType const& data,
                                  Day07AnswerType const& answer) {
  return answer + p2;
}
