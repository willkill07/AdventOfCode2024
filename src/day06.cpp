module;

#include <algorithm>
#include <bit>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include <ankerl/unordered_dense.h>
#include <unordered_set>

export module day06;

struct Guard {
  int x{0}, y{0}, dx{0}, dy{-1};

  constexpr Guard() noexcept = default;

  constexpr inline Guard(int x, int y) noexcept : x{x}, y{y} {
  }

  constexpr inline void Turn() noexcept {
    std::swap(dx, dy);
    dx = -dx;
  }

  constexpr inline Guard& Move() noexcept {
    std::tie(x, y) = std::pair(x + dx, y + dy);
    return *this;
  }

  constexpr bool operator==(Guard const& rhs) const noexcept {
    return std::bit_cast<__int128_t>(*this) == std::bit_cast<__int128_t>(rhs);
  }
};

struct GuardHash {
  using is_avalanching = void; //NOLINT(readability-identifier-naming)

  [[nodiscard]] auto operator()(Guard const& g) const noexcept -> std::size_t {
    static_assert(std::has_unique_object_representations_v<Guard>);
    return ankerl::unordered_dense::detail::wyhash::hash(&g, sizeof(g));
  }
};

struct Context {
  std::string buf;
  int dim;
  Guard g;

  constexpr Context() noexcept = default;
  Context(std::string_view data) noexcept
      : buf{data}, dim{static_cast<int>(data.find('\n')) + 1}, g{[&] {
          auto index = static_cast<int>(data.find('^'));
          return Guard{index % dim, index / dim};
        }()} {
    Update(g, 'X');
  }

  [[nodiscard]] constexpr char Map(Guard const& guard) const noexcept {
    if (guard.y >= 0 and guard.y < dim and guard.x >= 0 and guard.x < dim) {
      return buf[static_cast<std::uint32_t>(guard.y * dim + guard.x)];
    } else {
      return 0;
    }
  }

  constexpr void Update(Guard const& guard, char v) {
    buf[static_cast<std::uint32_t>(guard.y * dim + guard.x)] = v;
  }

  [[nodiscard]] constexpr char LookAhead(Guard const& guard) const noexcept {
    return Map(Guard{guard}.Move());
  }
};

export using Day06ParsedType = Context;
export using Day06AnswerType = int;

export Day06ParsedType Day06Parse(std::string_view input) noexcept {
  return Context{input};
}

constexpr auto Simulate = [](Context& ctx) {
  int total{0};
  Guard g{ctx.g};
  while (true) {
    if (ctx.Map(g) != 'x') {
      ++total;
      ctx.Update(g, 'x');
    }
    while (ctx.LookAhead(g) == '#') {
      g.Turn();
    }
    if (ctx.LookAhead(g) == 0) {
      break;
    }
    g.Move();
  }
  return total;
};

export Day06AnswerType Day06Part1(Day06ParsedType ctx) {
  return Simulate(ctx);
}

export Day06AnswerType Day06Part2(Day06ParsedType ctx, [[maybe_unused]] Day06AnswerType const& answer) {
  Simulate(ctx);
  int total{0};
  // std::unordered_set<Guard, GuardHash> history, shadow_history;
  ankerl::unordered_dense::set<Guard, GuardHash> history, shadow_history;
  Guard guard{ctx.g};
  while (true) {
    ctx.Update(guard, 'X');
    char g;
    while ((g = ctx.LookAhead(guard)) == '#') {
      history.insert(guard);
      guard.Turn();
    }
    if (g == 0) {
      break;
    } else if (g == 'x') {
      Guard wall{guard};
      wall.Move();
      ctx.Update(wall, '#');
      Guard shadow{guard};
      shadow.Turn();
      shadow_history.clear();
      char s;
      while ((s = ctx.LookAhead(shadow)) != 0) {
        if (s == '#') {
          if (history.contains(shadow) or shadow_history.contains(shadow)) {
            break;
          } else {
            shadow_history.insert(shadow);
          }
          shadow.Turn();
        } else {
          shadow.Move();
        }
      }
      total += (s != 0);
      ctx.Update(wall, 'x');
    }
    guard.Move();
  }
  return total;
}
