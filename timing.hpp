#pragma once

#include <format>

struct timing_stats {
  long parse{0};
  long part1{0};
  long part2{0};
  long total{0};

  constexpr inline timing_stats &
  operator+=(timing_stats const &other) noexcept {
    parse += other.parse;
    part1 += other.part1;
    part2 += other.part2;
    total += other.total;
    return *this;
  }

  [[nodiscard]] constexpr inline timing_stats
  operator+(timing_stats const &other) noexcept {
    return timing_stats{*this} += other;
  }
};

template <> struct std::formatter<timing_stats> {
  constexpr inline auto parse(std::format_parse_context &ctx) {
    return ctx.begin();
  }

  inline auto format(const timing_stats &obj, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "{:>12}µs{:>12}µs{:>12}µs{:>12}µs",
                          obj.parse, obj.part1, obj.part2, obj.total);
  }
};
