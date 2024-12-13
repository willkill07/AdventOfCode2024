module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

export module day13;

using Loc = std::pair<long, long>;

struct Game {
  Loc a, b, prize;
};

export using Day13ParsedType = std::vector<Game>;
export using Day13AnswerType = long;

export Day13ParsedType Day13Parse(std::string_view input) noexcept {
  auto games = ctre::multiline_search_all<
      R"(Button A: X\+(\d+), Y\+(\d+)\nButton B: X\+(\d+), Y\+(\d+)\nPrize: X=(\d+), Y=(\d+))">(input);
  auto parse = [](auto&& match) {
    auto [_, ax, ay, bx, by, px, py] = match;
    return Game{.a = Loc{ax.to_number(), ay.to_number()},
                .b = Loc{bx.to_number(), by.to_number()},
                .prize = Loc{px.to_number(), py.to_number()}};
  };
  return std::ranges::to<std::vector>(std::views::transform(games, parse));
}

static constexpr auto Div = []<std::integral T>(T a, std::same_as<T> auto b) {
  return std::pair{a / b, a % b};
};

static inline long Simulate(long offset, Game const& game) {
  auto const [btn_a, btn_b, prize] = game;
  auto const [ax, ay] = btn_a;
  auto const [bx, by] = btn_b;
  auto [x, y] = prize;
  x += offset;
  y += offset;
  if (long const det = by * ax - bx * ay; det == 0) {
    return 0L;
  } else if (auto const [bq, br] = Div(y * ax - x * ay, det); br != 0) {
    return 0L;
  } else if (auto const [aq, ar] = Div(x - bq * bx, ax); ar != 0) {
    return 0L;
  } else {
    return 3 * aq + bq;
  }
}

export Day13AnswerType Day13Part1(Day13ParsedType const& data) noexcept {
  return std::ranges::fold_left(std::views::transform(data, std::bind_front(Simulate, 0)), 0L, std::plus{});
}

export Day13AnswerType Day13Part2(Day13ParsedType const& data,
                                  [[maybe_unused]] Day13AnswerType const& answer) {
  return std::ranges::fold_left(
      std::views::transform(data, std::bind_front(Simulate, 10'000'000'000'000L)), 0L, std::plus{});
}
