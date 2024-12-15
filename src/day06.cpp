module;

#include <algorithm>
#include <ranges>
#include <string_view>
#include <utility>

#include <ankerl/unordered_dense.h>

#include "point.hpp"

export module day06;

import threading;

namespace {

struct Guard {
  Point loc{0, 0};
  Dir dir{Dir::Up};

  constexpr Guard() noexcept = default;

  constexpr inline Guard(Point const& l, Dir const& d = Dir::Up) noexcept : loc{l}, dir{d} {
  }

  constexpr inline void Turn() noexcept {
    dir = Dir((std::to_underlying(dir) + 1) & 0x3);
  }

  constexpr inline Guard Move() noexcept {
    Guard copy{*this};
    loc += dir;
    return copy;
  }

  [[nodiscard]] constexpr inline std::size_t Index(auto dim) const noexcept {
    return loc.Index(dim) + static_cast<std::size_t>(std::to_underlying(dir)) * dim * dim;
  }

  constexpr bool operator==(Guard const& rhs) const noexcept = default;
};

class JumpMap {
public:
  std::size_t dim;
  Guard guard;

private:
  std::string_view grid_;
  std::vector<int> mapping_;

  [[nodiscard]] constexpr inline auto GetOffset(Dir id) const noexcept {
    return dim * dim * static_cast<std::size_t>(std::to_underlying(id));
  }

public:
  [[nodiscard]] bool InBounds(Point const& pos) const noexcept {
    return 0 <= pos.x and std::cmp_less(pos.x, dim) and 0 <= pos.y and std::cmp_less(pos.y, dim);
  }

  [[nodiscard]] char Get(Point const& pos) const noexcept {
    return grid_[pos.Index(dim + 1)];
  }

  inline JumpMap(std::string_view grid)
      : dim{grid.find('\n')},
        guard{[&] {
          auto [y, x] = std::div(static_cast<int>(grid.find('^')), static_cast<int>(dim + 1));
          return Guard{Point{x, y}};
        }()},
        grid_{grid},
        mapping_(dim * dim * 4) {
    int const d{static_cast<int>(dim)};
    auto ordered = std::views::iota(0, d);
    for (int x : ordered) {
      for (int last{-1}; int y : ordered) {
        Point const p{x, y};
        if (Get(p) == '#') {
          last = y + 1;
        }
        mapping_[p.Index(dim) + GetOffset(Dir::Up)] = last;
      }
      for (int last{d}; int y : std::views::reverse(ordered)) {
        Point const p{x, y};
        if (Get(p) == '#') {
          last = y - 1;
        }
        mapping_[p.Index(dim) + GetOffset(Dir::Down)] = last;
      }
    }
    for (int y : ordered) {
      for (int last{-1}; int x : ordered) {
        Point const p{x, y};
        if (Get(p) == '#') {
          last = x + 1;
        }
        mapping_[p.Index(dim) + GetOffset(Dir::Left)] = last;
      }
      for (int last{d}; int x : std::views::reverse(ordered)) {
        Point const p{x, y};
        if (Get(p) == '#') {
          last = x - 1;
        }
        mapping_[p.Index(dim) + GetOffset(Dir::Right)] = last;
      }
    }
  }

  void Step(Guard& g, Point const& obstacle) const noexcept {
    auto& [pos, dir] = g;
    Point const end{obstacle - dir};
    switch (int const limit{mapping_[pos.Index(dim) + GetOffset(dir)]}; dir) {
    case Dir::Up:
      pos.y = (pos.x == obstacle.x and pos.y > obstacle.y and obstacle.y >= limit) ? end.y : limit;
      break;
    case Dir::Down:
      pos.y = (pos.x == obstacle.x and pos.y < obstacle.y and obstacle.y <= limit) ? end.y : limit;
      break;
    case Dir::Left:
      pos.x = (pos.y == obstacle.y and pos.x > obstacle.x and obstacle.x >= limit) ? end.x : limit;
      break;
    case Dir::Right:
      pos.x = (pos.y == obstacle.y and pos.x < obstacle.x and obstacle.x <= limit) ? end.x : limit;
      break;
    }
    g.Turn();
  }

  [[nodiscard]] bool inline HasCycle(Guard g, Point const& obstacle) const noexcept {
    thread_local ankerl::unordered_dense::set<int> seen;
    seen.clear();
    while (InBounds(g.loc)) {
      int const index{static_cast<int>(g.Index(dim))};
      if (seen.contains(index)) {
        return true;
      }
      seen.insert(index);
      Step(g, obstacle);
    }
    return false;
  }
};

} // namespace

export using Day06ParsedType = JumpMap;
export using Day06AnswerType = long;

export Day06ParsedType Day06Parse(std::string_view input) noexcept {
  return JumpMap{input};
}

static std::vector<Guard> path;

export Day06AnswerType Day06Part1(Day06ParsedType const& mapping) {
  path.reserve(6'000);
  path.clear();
  std::vector<char> visited(static_cast<std::size_t>(mapping.dim * mapping.dim), 0);
  Guard g{mapping.guard};
  long count{1};
  while (mapping.InBounds(g.loc)) {
    while (mapping.InBounds(g.loc + g.dir) and mapping.Get(g.loc + g.dir) == '#') {
      g.Turn();
    }
    Guard const curr{g.Move()};
    auto const& [pos, _] = g;
    if (std::size_t const idx{pos.Index(mapping.dim)};
        mapping.InBounds(pos) and mapping.Get(pos) == '.' and not visited[idx]) {
      visited[idx] = 1;
      ++count;
      path.push_back(curr);
    }
  }
  return count;
}

export Day06AnswerType Day06Part2(Day06ParsedType const& mapping,
                                  [[maybe_unused]] Day06AnswerType const& answer) {
  std::vector<char> added(static_cast<std::size_t>(mapping.dim * mapping.dim), '.');
  threading::ParallelForEach(path, [&](Guard const& guard) {
    Point const obstacle{guard.loc + guard.dir};
    if (mapping.InBounds(obstacle) and mapping.HasCycle(guard, obstacle)) {
      added[obstacle.Index(mapping.dim)] = 'X';
    }
  });
  return std::ranges::count(added, 'X');
}
