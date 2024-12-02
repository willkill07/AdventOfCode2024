#pragma once

#include <format>

struct TimingStats {
  static inline int repetitions{1};

  long parse{0};
  long part1{0};
  long part2{0};
  long total{0};

  constexpr inline TimingStats& operator+=(TimingStats const& other) noexcept {
    parse += other.parse;
    part1 += other.part1;
    part2 += other.part2;
    total += other.total;
    return *this;
  }

  [[nodiscard]] constexpr inline TimingStats operator+(TimingStats const& other) noexcept {
    return TimingStats{*this} += other;
  }
};

template <> struct std::formatter<TimingStats> {
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr inline auto parse(std::format_parse_context& ctx) {
    return ctx.begin();
  }

  // NOLINTNEXTLINE(readability-identifier-naming)
  inline auto format(const TimingStats& obj, std::format_context& ctx) const {
    return std::format_to(ctx.out(),
                          "{:>12}µs{:>12}µs{:>12}µs{:>12}µs",
                          obj.parse / TimingStats::repetitions,
                          obj.part1 / TimingStats::repetitions,
                          obj.part2 / TimingStats::repetitions,
                          obj.total / TimingStats::repetitions);
  }
};
