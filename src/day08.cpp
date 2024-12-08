module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>

export module day08;

namespace {

struct Point {
  int x{-1}, y{-1};

  [[nodiscard]] constexpr inline std::size_t Index(int width) const noexcept {
    return static_cast<std::size_t>(static_cast<std::int64_t>(y * width) + x);
  }

  constexpr inline Point& operator+=(Point const& other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
  }

  constexpr inline Point& operator-=(Point const& other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  [[nodiscard]] constexpr inline Point operator+(Point const& other) const noexcept {
    return Point{*this} += other;
  }

  [[nodiscard]] constexpr inline Point operator-(Point const& other) const noexcept {
    return Point{*this} -= other;
  }
};

constexpr static std::size_t MaxRepeats{4};
constexpr static char MinChar{'0'};
constexpr static char MaxChar{'z'};
static_assert(MinChar < MaxChar);
constexpr static std::size_t NumChars{MaxChar - MinChar + 1};

using Lookup = std::array<Point, MaxRepeats * NumChars>;

struct Info {
  Lookup table;
  int width;
};

constexpr auto InBounds = [](int dim, Point const& loc) {
  auto const& [x, y] = loc;
  return 0 <= x and x < dim and 0 <= y and y < dim;
};

} // namespace

export using Day08ParsedType = Info;
export using Day08AnswerType = long;

export Day08ParsedType Day08Parse(std::string_view input) noexcept {
  Lookup table;
  std::ranges::fill(table, Point{});
  std::array<std::size_t, NumChars> offsets;
  offsets.fill(0);
  int x{0}, y{0};
  for (char c : input) {
    if (c == '\n') {
      x = 0;
      ++y;
    } else {
      if (c != '.') {
        table[static_cast<std::size_t>(4 * (c - MinChar)) +
              offsets[static_cast<std::size_t>(c - MinChar)]++] = Point{x, y};
      }
      ++x;
    }
  }
  return Info{.table = table, .width = (static_cast<int>(input.size()) / y) - 1};
}

export Day08AnswerType Day08Part1(Day08ParsedType const& data) noexcept {
  std::vector<bool> locs(static_cast<std::size_t>(data.width * data.width), false);
  for (auto&& candidate : data.table | std::views::chunk(MaxRepeats)) {
    auto letter = std::views::take_while(candidate, std::bind_front(InBounds, data.width));
    for (auto const [idx, p0] : std::views::enumerate(letter)) {
      for (auto const p1 : std::views::drop(letter, idx + 1)) {
        Point const delta{p1 - p0};
        if (Point const an0{p0 - delta}; InBounds(data.width, an0)) {
          locs[an0.Index(data.width)] = true;
        }
        if (Point const an1{p1 + delta}; InBounds(data.width, an1)) {
          locs[an1.Index(data.width)] = true;
        }
      }
    }
  }
  return std::ranges::count(locs, true);
}

export Day08AnswerType Day08Part2(Day08ParsedType const& data,
                                  [[maybe_unused]] Day08AnswerType const& answer) {
  std::vector<bool> locs(static_cast<std::size_t>(data.width * data.width), false);
  for (auto&& candidate : data.table | std::views::chunk(MaxRepeats)) {
    auto letter = std::views::take_while(candidate, std::bind_front(InBounds, data.width));
    for (auto const [idx, p0] : std::views::enumerate(letter)) {
      for (auto const p1 : std::views::drop(letter, idx + 1)) {
        locs[p0.Index(data.width)] = true;
        locs[p1.Index(data.width)] = true;
        Point const delta{p1 - p0};
        Point an0{p0}, an1{p1};
        while (InBounds(data.width, an0 -= delta)) {
          locs[an0.Index(data.width)] = true;
        }
        while (InBounds(data.width, an1 += delta)) {
          locs[an1.Index(data.width)] = true;
        }
      }
    }
  }
  return std::ranges::count(locs, true);
}
