module;

#include <algorithm>
#include <atomic>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

export module day22;

import threading;

export using Day22ParsedType = std::vector<unsigned>;
export using Day22AnswerType = std::size_t;

export Day22ParsedType Day22Parse(std::string_view input) noexcept {
  return ctre::split<"\n">(input) |
         std::views::transform([](auto&& match) { return match.template to_number<unsigned>(); }) |
         std::ranges::to<std::vector>();
}

inline constexpr unsigned StepSecret(unsigned n) noexcept {
  n = (n ^ (n << 6)) & 0xFFFFFFU;
  n = (n ^ (n >> 5)) & 0xFFFFFFU;
  n = (n ^ (n << 11)) & 0xFFFFFFU;
  return n;
}

export Day22AnswerType Day22Part1(Day22ParsedType const& data) noexcept {
  return threading::ParallelReduceAdd(
      data,
      [&] [[gnu::flatten]] (unsigned m) {
        for (auto _ : std::views::iota(0, 2'000)) {
          m = StepSecret(m);
        }
        return m;
      },
      0LU);
}

static inline constexpr int Price(unsigned x) noexcept {
  return static_cast<int>(x % 10);
}

static inline constexpr int LastDigitChange(unsigned a, unsigned b) {
  return Price(b) - Price(a);
}

static inline constexpr unsigned Shift(auto val) noexcept {
  return static_cast<unsigned>(val + 9);
}

static inline constexpr unsigned ShiftWindow(unsigned prev, unsigned current, int next) noexcept {
  return 19U * current - (19U * 19U * 19U * 19U) * prev + Shift(next);
}

export int Day22Part2(Day22ParsedType const& data, [[maybe_unused]] Day22AnswerType const& answer) {
  std::array<int, 19LU * 19LU * 19LU * 19LU> seq;
  seq.fill(0);
  threading::ParallelForEach(data, [&] [[gnu::flatten]] (unsigned m) {
    std::array<bool, 19LU * 19LU * 19LU * 19LU> seen;
    seen.fill(false);
    std::array<unsigned, 2001> nums;
    {
      nums.front() = m;
      std::ranges::transform(std::views::iota(0U, 2'000U), std::next(std::begin(nums)), [&m](auto) {
        return (m = StepSecret(m));
      });
    }
    std::array<int, 2'000U> diffs;
    std::ranges::copy(std::views::adjacent_transform<2>(nums, LastDigitChange), std::begin(diffs));
    unsigned pattern{0};
    pattern = ShiftWindow(0, pattern, diffs[0]);
    pattern = ShiftWindow(0, pattern, diffs[1]);
    pattern = ShiftWindow(0, pattern, diffs[2]);
    for (unsigned prev{0}; unsigned const i : std::views::iota(3U, 2'000U)) {
      pattern = ShiftWindow(prev, pattern, diffs[i]);
      prev = Shift(diffs[i - 3]);
      if (not seen[pattern]) {
        seen[pattern] = true;
        std::atomic_ref{seq[pattern]}.fetch_add(Price(nums[i + 1]), std::memory_order_release);
      }
    }
  });
  return std::ranges::max(seq);
}
