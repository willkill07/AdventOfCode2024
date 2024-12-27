module;

#include <algorithm>
#include <atomic>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

export module day22;

import threading;

export using Day22ParsedType = std::pair<std::size_t, int>;

inline constexpr unsigned StepSecret(unsigned n) noexcept {
  n = (n ^ (n << 6)) & 0xFFFFFFU;
  n = (n ^ (n >> 5)) & 0xFFFFFFU;
  n = (n ^ (n << 11)) & 0xFFFFFFU;
  return n;
}

export Day22ParsedType Day22Parse(std::string_view input) noexcept {
  auto data = ctre::split<"\n">(input) |
              std::views::transform([](auto&& match) { return match.template to_number<unsigned>(); }) |
              std::ranges::to<std::vector>();

  std::array<int, 32LU * 32LU * 32LU * 32LU> seq;
  seq.fill(0);

  std::size_t const sum = threading::ParallelReduceAdd(
      data,
      [&](unsigned n) {
        std::array<bool, 32LU * 32LU * 32LU * 32LU> seen;
        seen.fill(false);

        std::array<unsigned char, 2'001LU> prices;
        {
          prices.front() = n % 10;
          std::ranges::transform(std::views::iota(0U, 2'000U), std::next(std::begin(prices)), [&](auto) {
            n = StepSecret(n);
            return n % 10;
          });
        }

        unsigned char prev_price{prices.front()};
        for (auto pattern{0U}; unsigned char const p : std::views::drop(prices, 1)) {
          pattern = ((pattern << 5) & 0xFFFFFU) | static_cast<unsigned>(9 + p - std::exchange(prev_price, p));
          if (not std::exchange(seen[pattern], true)) {
            std::atomic_ref{seq[pattern]}.fetch_add(p, std::memory_order_release);
          }
        }
        return n;
      },
      0ZU);
  return std::pair{sum, std::ranges::max(seq)};
}

export std::size_t Day22Part1(Day22ParsedType const& data) noexcept {
  return data.first;
}

export int Day22Part2(Day22ParsedType const& data, [[maybe_unused]] std::size_t const& answer) {
  return data.second;
}
