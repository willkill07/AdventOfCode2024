module;

#include <algorithm>
#include <concepts>
#include <functional>
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

constexpr auto Expand = [] [[gnu::always_inline]] (char c) {
  using std::string_view_literals::operator""sv;
  switch (c) { // clang-format off
    case 'O': return "[]"sv;
    case '@': return "@."sv;
    case '#': return "##"sv;
    case '.': return ".."sv;
  } // clang-format on
  __builtin_unreachable();
};

static constexpr auto SkipNewline = std::bind_front(std::not_equal_to{}, '\n');

static constexpr auto ToDir = [] [[gnu::always_inline]] (char m) {
  switch (m) { // clang-format off
    case '^': return Dir::Up;
    case '>': return Dir::Right;
    case 'v': return Dir::Down;
    case '<': return Dir::Left;
  } // clang-format on
  __builtin_unreachable();
};

export Day15ParsedType Day15Parse(std::string_view input) noexcept {
  using std::string_view_literals::operator""sv;
  int const width{static_cast<int>(input.find('\n'))};
  std::size_t const size{input.find("\n\n")};
  int const height{static_cast<int>(size + 1) / (width + 1)};

  std::string part1(static_cast<std::size_t>(width * height), '\0');
  std::string part2(static_cast<std::size_t>(width * height * 2), '\0');
  auto const data = input.substr(0, static_cast<std::size_t>(size));
  std::ranges::copy(std::views::filter(data, SkipNewline), part1.begin());
  std::ranges::copy(std::views::join(std::views::transform(std::views::filter(data, SkipNewline), Expand)),
                    part2.begin());

  std::string_view original_moves{input.substr(size + 2)};
  std::size_t const moves_width{original_moves.find('\n') + 1};
  std::size_t const move_rows{original_moves.size() / moves_width};
  std::vector<Dir> moves(original_moves.size() - move_rows);
  std::ranges::copy(std::views::transform(std::views::filter(original_moves, SkipNewline), ToDir),
                    moves.begin());

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
  auto loc = std::next(std::begin(grid), start);

  std::array const offsets{-width, 1, width, -1};
  auto offset_for = [&](Dir d) { return offsets[static_cast<unsigned char>(std::to_underlying(d))]; };

  for (int const offset : std::views::transform(simulation_data.moves, offset_for)) {
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
    *std::exchange(loc, check) = '.';
  }
  *loc = '@';
  return Score(grid, width, 'O');
}

export Day15AnswerType Day15Part2(Day15ParsedType const& simulation_data,
                                  [[maybe_unused]] Day15AnswerType const& answer) {
  auto [grid, start, width] = simulation_data.part2;
  auto loc = std::next(std::begin(grid), start);

  std::array const offsets{-width, 1, width, -1};
  auto offset_for = [&](Dir d) { return offsets[static_cast<unsigned char>(std::to_underlying(d))]; };

  std::vector<std::string::iterator> boxes;
  boxes.reserve(40); // never was above 34 for my input
  auto add_box = [&boxes] [[gnu::always_inline]] (std::string::iterator iter) {
    if (*iter == '[') {
      if (boxes.empty() or std::views::reverse(boxes)[1] != iter) {
        boxes.push_back(iter);
        boxes.push_back(iter + 1);
      }
    } else {
      if (boxes.empty() or std::views::reverse(boxes)[0] != iter) {
        boxes.push_back(iter - 1);
        boxes.push_back(iter);
      }
    }
  };

  for (int const offset : std::views::transform(simulation_data.moves, offset_for)) {
    auto check{loc + offset};
    if (*check == '#') {
      continue;
    }
    if (*check != '.') {
      if (offset & 1) {
        auto next{check + offset};
        while (*next == '[' or *next == ']') {
          next += offset;
        }
        if (*next == '#') {
          continue;
        }
        while (next != loc) {
          std::iter_swap(next, next -= offset);
        }
      } else {
        [&] {
          add_box(check);
          for (std::contiguous_iterator auto prev = boxes.begin(); prev != boxes.end();) {
            std::contiguous_iterator auto end = boxes.end();
            for (auto const box : std::ranges::subrange(prev, end)) {
              if (auto const next{box + offset}; *next != '.') {
                if (*next == '#') {
                  boxes.clear();
                  return;
                } else {
                  add_box(next);
                }
              }
            }
            prev = end;
          }
        }();
        if (boxes.empty()) {
          continue;
        }
        std::ranges::for_each(std::views::reverse(boxes), [&](auto b) { std::iter_swap(b, b + offset); });
        boxes.clear();
      }
    }
    *std::exchange(loc, check) = '.';
  }
  *loc = '@';
  return Score(grid, width, '[');
}
