

module;

#include <algorithm>
#include <format>
#include <limits>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

#include "circular_buffer.hpp"
#include "point.hpp"

export module day18;

namespace {
using Grid = std::array<std::array<int, 71U>, 71U>;

constexpr Point const origin{0, 0};
constexpr Point const end{70, 70};

} // namespace

export using Day18ParsedType = Grid;

export Day18ParsedType Day18Parse(std::string_view input) noexcept {
  Grid grid;
  for (auto& row : grid) {
    row.fill(std::numeric_limits<int>::max());
  }
  for (auto [i, match] : std::views::enumerate(ctre::search_all<R"((\d+),(\d+)\n)">(input))) {
    auto [_, x, y] = match;
    grid[y.to_number<unsigned>()][x.to_number<unsigned>()] = i;
  }
  return grid;
}

export unsigned Day18Part1(Grid grid) noexcept {
  CircularBuffer<std::pair<Point, unsigned>> todo{96};

  grid[static_cast<unsigned>(origin.y)][static_cast<unsigned>(origin.x)] = 0;
  todo.push_back(std::pair{origin, 0});
  while (not todo.empty()) {
    auto [pos, cost] = todo.front();
    todo.pop_front();
    if (pos == end) {
      return cost;
    }
    for (auto delta : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
      if (Point const next{pos + delta}; 0 <= next.x and next.x < 71 and 0 <= next.y and next.y < 71) {
        if (int& curr = grid[static_cast<unsigned>(next.y)][static_cast<unsigned>(next.x)]; curr > 1'024) {
          curr = 0;
          todo.push_back(std::pair{next, cost + 1});
        }
      }
    }
  }
  __builtin_unreachable();
}

export std::string Day18Part2(Grid grid, [[maybe_unused]] unsigned const& answer) {
  int time{std::numeric_limits<int>::max()};
  std::vector<std::pair<int, Point>> heap;
  heap.reserve(16);
  CircularBuffer<Point> todo{1'344};
  grid[0][0] = 0;
  todo.push_back(origin);
  while (true) {
    while (not todo.empty()) {
      Point const pos{todo.front()};
      todo.pop_front();
      if (pos == end) {
        for (auto [x, y] :
             std::views::cartesian_product(std::views::iota(0U, 71U), std::views::iota(0U, 71U))) {
          if (grid[y][x] == time) {
            return std::format("{},{}", x, y);
          }
        }
      }
      for (Dir const delta : {Dir::Left, Dir::Right, Dir::Up, Dir::Down}) {
        if (Point const next{pos + delta}; 0 <= next.x and next.x < 71 and 0 <= next.y and next.y < 71) {
          if (int& curr = grid[static_cast<unsigned>(next.y)][static_cast<unsigned>(next.x)]; time < curr) {
            curr = 0;
            todo.push_back(next);
          } else {
            heap.emplace_back(curr, next);
            std::ranges::push_heap(heap);
          }
        }
      }
    }
    std::ranges::pop_heap(heap);
    auto [t, saved] = heap.back();
    heap.pop_back();
    time = t;
    todo.push_back(saved);
  }
}
