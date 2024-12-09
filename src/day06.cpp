module;

#include <algorithm>
#include <atomic>
#include <ranges>
#include <string_view>
#include <thread>
#include <utility>

#include <ankerl/unordered_dense.h>
#include <thread_pool/thread_pool.h>

export module day06;

namespace {

enum class Dir { Up = 0, Right = 1, Down = 2, Left = 3 };

struct Point {
  int x, y;

  constexpr inline Point(int x, int y) noexcept : x{x}, y{y} {
  }
  constexpr inline Point(Dir dir) noexcept : Point(0, 0) {
    switch (dir) {
    case Dir::Up:
      y = -1;
      break;
    case Dir::Down:
      y = 1;
      break;
    case Dir::Left:
      x = -1;
      break;
    case Dir::Right:
      x = 1;
      break;
    }
  }

  constexpr inline Point& operator+=(Point const& p) noexcept {
    x += p.x;
    y += p.y;
    return *this;
  }

  constexpr inline Point& operator+=(Dir dir) noexcept {
    return *this += Point{dir};
    switch (dir) {
    case Dir::Down:
      ++y;
      break;
    case Dir::Up:
      --y;
      break;
    case Dir::Left:
      --x;
      break;
    case Dir::Right:
      ++x;
      break;
    }
    return *this;
  }

  constexpr inline Point& operator-=(Dir dir) noexcept {
    return *this -= Point{dir};
    switch (dir) {
    case Dir::Down:
      --y;
      break;
    case Dir::Up:
      ++y;
      break;
    case Dir::Left:
      ++x;
      break;
    case Dir::Right:
      --x;
      break;
    }
    return *this;
  }

  constexpr inline Point& operator-=(Point const& p) noexcept {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  [[nodiscard]] constexpr inline Point operator+(Point const& p) const noexcept {
    return Point{*this} += p;
  }

  [[nodiscard]] constexpr inline Point operator+(Dir dir) const noexcept {
    return Point{*this} += dir;
  }

  [[nodiscard]] constexpr inline Point operator-(Point const& p) const noexcept {
    return Point{*this} -= p;
  }

  [[nodiscard]] constexpr inline Point operator-(Dir dir) const noexcept {
    return Point{*this} -= dir;
  }

  [[nodiscard]] constexpr inline bool operator==(Point const& rhs) const noexcept = default;

  [[nodiscard]] constexpr inline std::size_t Index(auto dim) const noexcept {
    return static_cast<std::size_t>(dim) * static_cast<std::size_t>(y) + static_cast<std::size_t>(x);
  }
};

struct Guard {
  Point loc{0, 0};
  Dir dir{Dir::Up};

  constexpr Guard() noexcept = default;

  constexpr inline Guard(Point const& l, Dir const& d = Dir::Up) noexcept : loc{l}, dir{d} {
  }

  constexpr inline void Turn() noexcept {
    dir = Dir{(std::to_underlying(dir) + 1) & 0x3};
  }

  constexpr inline Guard& Move() noexcept {
    loc += dir;
    return *this;
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

  void Step(Guard& g) const noexcept {
    std::size_t const index{g.loc.Index(dim)};
    switch (g.dir) {
    case Dir::Up:
      g.loc.y = up_[index];
      break;
    case Dir::Down:
      g.loc.y = down_[index];
      break;
    case Dir::Left:
      g.loc.x = left_[index];
      break;
    case Dir::Right:
      g.loc.x = right_[index];
      break;
    }
    g.Turn();
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

  [[nodiscard]] bool inline HasCycle(Guard g) const noexcept {
    thread_local ankerl::unordered_dense::set<int> seen;
    seen.clear();
    Point const obstacle{g.loc + g.dir};
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

template <std::ranges::random_access_range Range>
void ParallelForEach(Range&& range,
                     std::move_only_function<void(std::ranges::range_const_reference_t<Range>)> fn) {
  static unsigned const THREADS{std::thread::hardware_concurrency()};
  static dp::thread_pool<> thread_pool{THREADS};
  std::size_t const n{range.size()};
  long curr{0};
  for (unsigned i : std::views::iota(0u, THREADS)) {
    long const next{static_cast<long>((i + 1) * n / THREADS)};
    thread_pool.enqueue_detach([&fn, &range, curr, next]() {
      for (Guard& item : std::views::take(std::views::drop(range, curr), next - curr)) {
        std::invoke(fn, item);
      }
    });
    curr = next;
  }
  thread_pool.wait_for_tasks();
}

} // namespace

export using Day06ParsedType = JumpMap;
export using Day06AnswerType = long;

export Day06ParsedType Day06Parse(std::string_view input) noexcept {
  return JumpMap{input};
}

static std::vector<Guard> path;

export Day06AnswerType Day06Part1(Day06ParsedType const& mapping) {
  std::vector<bool> visited(static_cast<std::size_t>(mapping.dim * mapping.dim), false);
  path.reserve(6'144);
  path.clear();
  Guard guard{mapping.guard};
  while (mapping.InBounds(guard.loc)) {
    auto const [start, dir] = guard;
    mapping.Step(guard);
    Point const stop{guard.loc};
    for (Point p{start}; p != stop; p += dir) {
      visited[p.Index(mapping.dim)] = true;
      path.emplace_back(p, dir);
    }
  }
  return std::ranges::count(visited, true);
}

export Day06AnswerType Day06Part2(Day06ParsedType const& mapping,
                                  [[maybe_unused]] Day06AnswerType const& answer) {
  std::vector<bool> visited(static_cast<std::size_t>(mapping.dim * mapping.dim), false);
  std::atomic_long count{0};
  ParallelForEach(path, [&](Guard const& guard) { count += mapping.HasCycle(guard); });
  return count.load();
}
