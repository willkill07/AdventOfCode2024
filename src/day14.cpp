
module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>

#include <ctre.hpp>

export module day14;

constexpr int width = 101;
constexpr int height = 103;
constexpr int inverse = 51; // W^-1 % H

constexpr unsigned MAX_SIZE{512};

static constexpr auto EuclidRem = []<typename T>(T a, std::same_as<T> auto b) {
  T v = a % b;
  return v < 0 ? v + b : v;
};

struct Robots {
  using Vec = std::array<int, MAX_SIZE>;
  alignas(64) Vec px, py, vx, vy;
  unsigned count{0};

  [[nodiscard]] auto StepX(int t) const {
    return std::views::transform(std::views::iota(0u, count),
                                 [this, t](unsigned i) { return EuclidRem(px[i] + vx[i] * t, width); });
  }

  [[nodiscard]] auto StepY(int t) const {
    return std::views::transform(std::views::iota(0u, count),
                                 [this, t](unsigned i) { return EuclidRem(py[i] + vy[i] * t, height); });
  }
};

export using Day14ParsedType = Robots;
export using Day14AnswerType = long;

export Day14ParsedType Day14Parse(std::string_view input) noexcept {
  Robots robots;
  unsigned i{0};
  for (auto [_, px, py, vx, vy] :
       ctre::multiline_search_all<R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+)\n)">(input)) {
    robots.px[i] = px.to_number();
    robots.py[i] = py.to_number();
    robots.vx[i] = vx.to_number();
    robots.vy[i] = vy.to_number();
    ++i;
  };
  robots.count = i;
  return robots;
}

static constexpr auto CountQuadrants = [](auto&& xrng, auto&& yrng) {
  constexpr int X{width / 2};
  constexpr int Y{height / 2};
  std::array<int, 4> quads{0, 0, 0, 0};
  for (auto [x, y] : std::views::zip(xrng, yrng)) {
    if (x != X and y != Y) {
      ++quads[static_cast<unsigned>((2 * (y > Y)) | (x > X))];
    }
  }
  return quads[0] * quads[1] * quads[2] * quads[3];
};

export Day14AnswerType Day14Part1(Day14ParsedType const& robots) noexcept {
  return CountQuadrants(robots.StepX(100), robots.StepY(100));
}

template <auto StepFn>
static constexpr auto CalcMinSpread = [](int bounds, Robots const& robots) {
  auto calc = [&](int t) {
    return std::ranges::fold_left(std::invoke(StepFn, robots, t), 0, [&](int sum, int curr) {
      int const diff{curr - bounds / 2};
      return sum + diff * diff;
    });
  };
  return std::views::enumerate(std::views::transform(std::views::iota(0, bounds), calc));
};

template <std::size_t I>
static constexpr auto Min = [](auto const& min, auto const& curr) {
  return std::min(min, curr, [](auto const& x, auto const& y) { return std::get<I>(x) < std::get<I>(y); });
};

export Day14AnswerType Day14Part2(Day14ParsedType const& robots,
                                  [[maybe_unused]] Day14AnswerType const& answer) {
  auto const best_x = *std::ranges::fold_left_first(CalcMinSpread<&Robots::StepX>(width, robots), Min<1>);
  auto const best_y = *std::ranges::fold_left_first(CalcMinSpread<&Robots::StepY>(height, robots), Min<1>);
  int const bx{static_cast<int>(std::get<0>(best_x))};
  int const by{static_cast<int>(std::get<0>(best_y))};
  return bx + EuclidRem(inverse * (by - bx), height) * width;
}
