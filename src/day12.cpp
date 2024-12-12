
module;

#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

#include "circular_buffer.hpp"
#include "point.hpp"

export module day12;

constexpr std::uint8_t Mask(Dir dir) noexcept {
  return static_cast<std::uint8_t>(1 << static_cast<int>(std::to_underlying(dir)));
}

constexpr static std::uint8_t North{Mask(Dir::Up)};
constexpr static std::uint8_t South{Mask(Dir::Down)};
constexpr static std::uint8_t West{Mask(Dir::Left)};
constexpr static std::uint8_t East{Mask(Dir::Right)};
constexpr static std::uint8_t NorthWest{North | West};
constexpr static std::uint8_t NorthEast{North | East};
constexpr static std::uint8_t SouthWest{South | West};
constexpr static std::uint8_t SouthEast{South | East};

static inline char Same(std::string_view map, int N, Point const& p) {
  if (0 <= p.x and p.x < N and 0 <= p.y and p.y < N) {
    return map[p.Index(N + 1)];
  } else {
    return 0;
  }
}

struct ShapeStats {
  int area;
  int perimeter;
  int sides;

  [[nodiscard]] int Part1() const noexcept {
    return area * perimeter;
  }

  [[nodiscard]] int Part2() const noexcept {
    return area * sides;
  }
};

export using Day12ParsedType = std::vector<ShapeStats>;
export using Day12AnswerType = long;

export Day12ParsedType Day12Parse(std::string_view input) noexcept {
  int const n{static_cast<int>(input.find('\n'))};

  std::vector<ShapeStats> stats;
  stats.reserve(500);

  CircularBuffer<Point> stack{256};
  std::vector<char> seen(static_cast<std::size_t>(n * n), false);
  std::vector<std::uint8_t> nb(static_cast<std::size_t>(n * n), 0);

  for (int y = 0; y < n; ++y) {
    for (int x = 0; x < n; ++x) {

      Point point{x, y};
      if (not seen[point.Index(n)]) {
        int cells{0}, edges{0}, corners{0};
        seen[point.Index(n)] = true;
        const char plant = input[point.Index(n + 1)];

        while (true) {
          std::size_t const idx{point.Index(n)};
          ++cells;
          // Part 1 solver -- count edges
          for (Dir const dir : {Dir::Up, Dir::Right, Dir::Down, Dir::Left}) {
            if (Point const next{point + dir}; Same(input, n, next) == plant) {
              nb[idx] |= Mask(dir);
              if (not std::exchange(seen[next.Index(n)], true)) {
                stack.push_back(next);
              }
            } else {
              ++edges;
            }
          }
          // Part 2 solver -- triple-count corners
          for (Dir const dx : {Dir::Left, Dir::Right}) {
            for (Dir const dy : {Dir::Up, Dir::Down}) {
              auto const mask{Mask(dx) | Mask(dy)};
              switch (nb[idx] & mask) {
              case 0:
                corners += 3;
                break;
              case North:
              case South:
              case East:
              case West:
                corners += Same(input, n, point + dx + dy) == plant;
                break;
              case NorthEast:
              case NorthWest:
              case SouthEast:
              case SouthWest:
                corners += Same(input, n, point + dx + dy) != plant;
                break;
              }
            }
          }
          if (stack.empty()) {
            break;
          } else {
            point = stack.back();
            stack.pop_back();
          }
        }
        stats.emplace_back(cells, edges, corners / 3);
      }
    }
  }
  return stats;
}

export Day12AnswerType Day12Part1(Day12ParsedType const& data) noexcept {
  return std::ranges::fold_left(std::views::transform(data, &ShapeStats::Part1), 0, std::plus{});
}

export Day12AnswerType Day12Part2(Day12ParsedType const& data,
                                  [[maybe_unused]] Day12AnswerType const& answer) {
  return std::ranges::fold_left(std::views::transform(data, &ShapeStats::Part2), 0, std::plus{});
}
