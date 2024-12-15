module;

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "point.hpp"

export module day15;

namespace {

struct GridData {
  std::string grid;
  int start;
  int width;
};

struct Simulation {
  GridData part1;
  GridData part2;
  std::vector<Dir> moves;
};

} // namespace

export using Day15ParsedType = Simulation;
export using Day15AnswerType = int;

static constexpr auto Div = []<std::integral T>(T a, std::same_as<T> auto b) {
  return std::pair{a / b, a % b};
};

export Day15ParsedType Day15Parse(std::string_view input) noexcept {
  int const width{static_cast<int>(input.find('\n'))};
  std::size_t const size{input.find("\n\n")};
  int const height{static_cast<int>(size + 1) / (width + 1)};

  std::string part1(static_cast<std::size_t>(width * height), '\0');
  std::string part2(static_cast<std::size_t>(width * height * 2), '\0');

  for (auto i1 = part1.begin(), i2 = part2.begin();
       char c : input.substr(0, static_cast<std::size_t>(size))) {
    // clang-format off
    switch (c) {
      case 'O': *i1++ = 'O'; *i2++ = '['; *i2++ = ']'; break;
      case '@': *i1++ = '@'; *i2++ = '@'; *i2++ = '.'; break;
      case '#': *i1++ = '#'; *i2++ = '#'; *i2++ = '#'; break;
      case '.': *i1++ = '.'; *i2++ = '.'; *i2++ = '.'; break;
      default: break;
    }
    // clang-format on
  }

  std::string_view original_moves{input.substr(size + 2)};
  std::size_t const moves_width{original_moves.find('\n') + 1};
  std::size_t const move_rows{original_moves.size() / moves_width};
  std::vector<Dir> moves(original_moves.size() - move_rows);
  for (auto i = moves.begin(); char m : original_moves) {
    // clang-format off
    switch (m) {
      case '^': *i++ = Dir::Up; break;
      case '>': *i++ = Dir::Right; break;
      case 'v': *i++ = Dir::Down; break;
      case '<': *i++ = Dir::Left; break;
      default: break;
    }
    // clang-format on
  }

  int const offset{static_cast<int>(input.find('@'))};
  auto const [y, x] = Div(offset, width + 1);

  return Simulation{.part1 = GridData{.grid = std::move(part1),
                                      .start = static_cast<int>(Point{x, y}.Index(width)),
                                      .width = width},
                    .part2 = GridData{.grid = std::move(part2),
                                      .start = static_cast<int>(Point{x * 2, y}.Index(width * 2)),
                                      .width = width * 2},
                    .moves = std::move(moves)};
}

static inline int Score(std::string_view grid, int width, char target) {
  int sum{0};
  for (int row{-1}; auto&& line : std::views::chunk(grid, width)) {
    ++row;
    for (int col{-1}; char const c : line) {
      ++col;
      if (c == target) {
        sum += (100 * row) + col;
      }
    }
  }
  return sum;
}

export Day15AnswerType Day15Part1(Day15ParsedType const& simulation_data) noexcept {
  auto [grid, start, width] = simulation_data.part1;
  auto loc = std::next(grid.begin(), start);

  auto offset_for = [&](Dir d) {
    static std::array arr{-width, 1, width, -1};
    return arr[static_cast<unsigned char>(std::to_underlying(d))];
  };

  for (Dir d : simulation_data.moves) {
    int const offset{offset_for(d)};
    auto check{loc + offset};
    if (*check == '#') {
      continue;
    }
    if (*check == 'O') {
      auto next{check + offset};
      while (*next == 'O') {
        next += offset;
      }
      if (*next == '#') {
        continue;
      }
      *next = 'O';
    }
    *check = '@';
    *loc = '.';
    loc = check;
  }
  return Score(grid, width, 'O');
}

export Day15AnswerType Day15Part2(Day15ParsedType const& simulation_data,
                                  [[maybe_unused]] Day15AnswerType const& answer) {
  auto [grid, start, width] = simulation_data.part2;
  auto loc = std::next(grid.begin(), start);

  auto offset_for = [&](Dir d) {
    static std::array arr{-width, 1, width, -1};
    return arr[static_cast<unsigned char>(std::to_underlying(d))];
  };

  std::vector<std::string::iterator> boxes;
  boxes.reserve(40); // never was above 34 for my input

  for (Dir d : simulation_data.moves) {
    int const offset{offset_for(d)};
    auto check{loc + offset};
    if (*check == '#') {
      continue;
    }
    if (*check != '.') {
      if (d == Dir::Left or d == Dir::Right) {
        // Easy mode -- scan until we don't see a box

        auto next{check + offset};
        while (*next == '[' or *next == ']') {
          next += offset;
        }

        // We encountered a wall -- abort
        if (*next == '#') {
          continue;
        }

        // Next must have been a space
        while (next != loc) {
          std::iter_swap(next, next - offset);
          next -= offset;
        }
      } else {
        // Hard mode -- potentially need to move many (or no) boxes

        // Provide a convenience function to enqueue new boxes to consider
        auto add_box = [&boxes](std::string::iterator iter) {
          // If the one we are adding was just added (lookback size of two) then skip
          if (std::ranges::contains(std::views::take(std::views::reverse(boxes), 2), iter)) {
            return;
          }
          // Always ensure insertion in sorted order
          if (*iter == '[') { 
            boxes.push_back(iter);
            boxes.push_back(iter + 1);
          } else {
            boxes.push_back(iter - 1);
            boxes.push_back(iter);
          }
        };

        // Enqueue all boxes that should move
        [&] {
          add_box(check);
          // While we have more to check
          for (auto prev = boxes.begin(); prev != boxes.end();) {
            auto end = boxes.end();
            for (auto const box : std::ranges::subrange(prev, end)) {
              if (auto const next{box + offset}; *next == '.') [[likely]] {
                // Nothing to do, but still in a valid state!
              } else if (*next == '#') {
                // Encountered a wall -- abort immediately
                boxes.clear();
                return;
              } else {
                // Add one new box (two iterators)
                add_box(next);
              }
            }
            prev = end;
          }
        }();

        // This means we hit a wall and should abort
        if (boxes.empty()) {
          continue;
        }

        // Move all of the boxes
        std::ranges::for_each(std::views::reverse(boxes), [&](auto b) { std::iter_swap(b, b + offset); });
        boxes.clear();
      }
    }

    // Perform the move
    *check = '@';
    *loc = '.';
    loc = check;
  }
  return Score(grid, width, '[');
}
