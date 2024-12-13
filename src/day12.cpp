module;

#include <array>
#include <string_view>
#include <utility>
#include <vector>

#include "point.hpp"

export module day12;

constexpr Dir RotateClockwise(Dir dir) noexcept {
  return static_cast<Dir>((std::to_underlying(dir) + 1) & 3);
}

constexpr Dir RotateCounterClockwise(Dir dir) noexcept {
  return static_cast<Dir>((4 + std::to_underlying(dir) - 1) & 3);
}

static inline char Get(std::string_view map, int N, Point const& p) {
  if (0 <= p.x and p.x < N and 0 <= p.y and p.y < N) {
    return map[p.Index(N + 1)];
  } else {
    return 0;
  }
}

export using Day12ParsedType = std::pair<int, int>;
export using Day12AnswerType = int;

export Day12ParsedType Day12Parse(std::string_view input) noexcept {
  int const n{static_cast<int>(input.find('\n'))};
  int part1{0}, part2{0};
  std::vector<char> seen(static_cast<std::size_t>(n * n), false);
  std::array<Point, 256> stack;
  unsigned index{0};
  auto push = [&](Point const& p) { stack[index++] = p; };
  auto pop = [&]() { return stack[--index]; };

  for (int y = 0; y < n; ++y) {
    for (int x = 0; x < n; ++x) {
      if (Point const point{x, y}; not seen[point.Index(n)]) {
        seen[point.Index(n)] = true;
        int cells{0}, edges{0}, corners{0};
        char const plant{input[point.Index(n + 1)]};
        push(point);
        while (index != 0) {
          Point const curr = pop();
          ++cells;
          #pragma unroll
          for (Dir const dir : {Dir::Down, Dir::Right, Dir::Up, Dir::Left}) {
            if (Point const next{curr + dir}; Get(input, n, next) == plant) {
              if (not seen[next.Index(n)]) {
                seen[next.Index(n)] = true;
                push(next);
              }
            } else {
              // Part 1 solver -- count edges
              ++edges;
              // Part 2 solver -- count corners (double-counted)
              Dir const ccw{RotateCounterClockwise(dir)};
              corners += (Get(input, n, curr + ccw) != plant or Get(input, n, curr + dir + ccw) == plant);
              Dir const cw{RotateClockwise(dir)};
              corners += (Get(input, n, curr + cw) != plant or Get(input, n, curr + dir + cw) == plant);
            }
          }
        }
        part1 += cells * edges;
        part2 += cells * (corners / 2);
      }
    }
  }
  return std::pair{part1, part2};
}

export Day12AnswerType Day12Part1(Day12ParsedType const& data) noexcept {
  return data.first;
}

export Day12AnswerType Day12Part2(Day12ParsedType const& data,
                                  [[maybe_unused]] Day12AnswerType const& answer) {
  return data.second;
}
