module;

#include <algorithm>
#include <atomic>
#include <ranges>
#include <string_view>
#include <utility>

#include <ankerl/unordered_dense.h>

#include "point.hpp"
#include "threading.hpp"

export module day06;

namespace {

struct Guard {
  Point loc{0, 0};
  Dir dir{Dir::Up};

  constexpr Guard() noexcept = default;

  constexpr inline Guard(Point const& l, Dir const& d = Dir::Up) noexcept : loc{l}, dir{d} {
  }

  constexpr inline void Turn() noexcept {
    dir = Dir{(std::to_underlying(dir) + 1) & 0x3};
  }

  constexpr inline Guard Move() noexcept {
    Guard copy{*this};
    loc += dir;
    return copy;
  }

  [[nodiscard]] constexpr inline std::size_t Index(auto dim) const noexcept {
    return loc.Index(dim) * 4LU + static_cast<std::size_t>(std::to_underlying(dir));
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
  std::span<int> up_, right_, down_, left_;

  [[nodiscard]] constexpr inline auto GetDir(this auto&& self, Dir id) noexcept {
    return std::span{self.mapping_}.subspan(
        self.dim * self.dim * static_cast<std::size_t>(std::to_underlying(id)), self.dim * self.dim);
  }

public:
  inline JumpMap(std::string_view grid)
      : dim{grid.find('\n')},
        guard{[&] {
          auto [y, x] = std::div(static_cast<int>(grid.find('^')), static_cast<int>(dim + 1));
          return Guard{Point{x, y}};
        }()},
        grid_{grid},
        mapping_(dim * dim * 4),
        up_{GetDir(Dir::Up)},
        right_{GetDir(Dir::Right)},
        down_{GetDir(Dir::Down)},
        left_{GetDir(Dir::Left)} {
    int const d{static_cast<int>(dim)};
    auto ordered = std::views::iota(0, d);
    for (int x : ordered) {
      std::ranges::fold_left(ordered, -1, [&](int last, int y) {
        Point const p{x, y};
        if (grid[p.Index(dim + 1)] == '#') {
          last = y + 1;
        }
        return up_[p.Index(dim)] = last;
      });
      std::ranges::fold_right(ordered, d, [&](int y, int last) {
        Point const p{x, y};
        if (grid[p.Index(dim + 1)] == '#') {
          last = y - 1;
        }
        return down_[p.Index(dim)] = last;
      });
    }
    for (int y : ordered) {
      std::ranges::fold_left(ordered, -1, [&](int last, int x) {
        Point const p{x, y};
        if (grid[p.Index(dim + 1)] == '#') {
          last = x + 1;
        }
        return left_[p.Index(dim)] = last;
      });
      std::ranges::fold_right(ordered, d, [&](int x, int last) {
        Point const p{x, y};
        if (grid[p.Index(dim + 1)] == '#') {
          last = x - 1;
        }
        return right_[p.Index(dim)] = last;
      });
    }
  }

  [[nodiscard]] bool InBounds(Point const& pos) const noexcept {
    return 0 <= pos.x and std::cmp_less(pos.x, dim) and 0 <= pos.y and std::cmp_less(pos.y, dim);
  }

  [[nodiscard]] char Get(Point const& pos) const noexcept {
    return grid_[pos.Index(dim + 1)];
  }

  void Step(Guard& g, Point const& obstacle) const noexcept {
    auto& [pos, dir] = g;
    Point const end{obstacle - dir};
    switch (std::size_t const index{pos.Index(dim)}; dir) {
    case Dir::Up: {
      int const ny{up_[index]};
      pos.y = (pos.x == obstacle.x and pos.y > obstacle.y and obstacle.y >= ny) ? end.y : ny;
      break;
    }
    case Dir::Down: {
      int const ny{down_[index]};
      pos.y = (pos.x == obstacle.x and pos.y < obstacle.y and obstacle.y <= ny) ? end.y : ny;
      break;
    }
    case Dir::Left: {
      int const nx{left_[index]};
      pos.x = (pos.y == obstacle.y and pos.x > obstacle.x and obstacle.x >= nx) ? end.x : nx;
      break;
    }
    case Dir::Right: {
      int const nx{right_[index]};
      pos.x = (pos.y == obstacle.y and pos.x < obstacle.x and obstacle.x <= nx) ? end.x : nx;
      break;
    }
    }
    g.Turn();
  }

  [[nodiscard]] bool inline HasCycle(Guard g, Point const& obstacle) const noexcept {
    thread_local ankerl::unordered_dense::set<int> seen;
    seen.clear();
    while (InBounds(g.loc)) {
      std::size_t const index{g.Index(dim)};
      if (auto [_, inserted] = seen.insert(index); not inserted) {
        return true;
      } else {
        Step(g, obstacle);
      }
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
  ParallelForEach(path, [&](Guard const& guard) {
    Point const obstacle{guard.loc + guard.dir};
    if (mapping.InBounds(obstacle) and mapping.HasCycle(guard, obstacle)) {
      std::atomic_ref{added[obstacle.Index(mapping.dim)]} = 'X';
    }
  });
  return std::ranges::count(added, 'X');
}
