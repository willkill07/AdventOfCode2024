module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

#include "point.hpp"

export module day14;

static constexpr int width{101}, height{103}, inverse{51};

static constexpr auto EuclidRem = []<std::integral T>(T a, std::same_as<T> auto b) {
  T const v{a % b};
  return v < 0 ? v + b : v;
};

struct Robot {
  Point pos, vel;
};

template <auto Acc, int Bounds>
static constexpr auto Step = [](std::vector<Robot> const& rng, int t) {
  return std::views::transform(rng, [t](Robot const& robot) {
    return EuclidRem(std::invoke(Acc, robot.pos) + t * std::invoke(Acc, robot.vel), Bounds);
  });
};

export using Day14ParsedType = std::vector<Robot>;
export using Day14AnswerType = int;

export Day14ParsedType Day14Parse(std::string_view input) noexcept {
  return std::ranges::to<std::vector>(std::views::transform(
      ctre::multiline_search_all<R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+)\n)">(input), [](auto&& match) {
        auto [_, px, py, vx, vy] = match;
        return Robot{Point{px.to_number(), py.to_number()}, Point{vx.to_number(), vy.to_number()}};
      }));
}

template <int MidX, int MidY>
static constexpr auto CountQuadrants = [](auto&& xrng, auto&& yrng) {
  std::array quads{0, 0, 0, 0};
  for (auto [x, y] : std::views::zip(xrng, yrng)) {
    if (x != MidX and y != MidY) {
      ++quads[static_cast<unsigned>((2 * (y > MidY)) | (x > MidX))];
    }
  }
  return *std::ranges::fold_left_first(quads, std::multiplies{});
};

export Day14AnswerType Day14Part1(Day14ParsedType const& robots) noexcept {
  return CountQuadrants<width / 2, height / 2>(Step<&Point::x, width>(robots, 100),
                                               Step<&Point::y, height>(robots, 100));
}

template <auto Acc, int Bounds>
static constexpr auto SpreadScore = [](std::vector<Robot> const& robots) {
  auto calc = [&](int t) {
    return std::ranges::fold_left(Step<Acc, Bounds>(robots, t), 0, [&](auto sum, int v) {
      int const diff{v - Bounds / 2};
      return sum + diff * diff;
    });
  };
  return std::views::enumerate(std::views::transform(std::views::iota(0, Bounds), calc));
};

template <std::size_t I>
static constexpr auto TupleMin = [](auto const& min, auto const& curr) {
  return std::min(min, curr, [](auto const& x, auto const& y) { return std::get<I>(x) < std::get<I>(y); });
};

export Day14AnswerType Day14Part2(Day14ParsedType const& robots,
                                  [[maybe_unused]] Day14AnswerType const& answer) {
  auto const best_x = *std::ranges::fold_left_first(SpreadScore<&Point::x, width>(robots), TupleMin<1>);
  auto const best_y = *std::ranges::fold_left_first(SpreadScore<&Point::y, height>(robots), TupleMin<1>);
  int const bx{static_cast<int>(std::get<0>(best_x))};
  int const by{static_cast<int>(std::get<0>(best_y))};
  return bx + EuclidRem(inverse * (by - bx), height) * width;
}
