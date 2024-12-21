#pragma once

#include <concepts>
#include <cstddef>

enum class Dir : signed char { Up = 0, Right = 1, Down = 2, Left = 3 };

struct Point {
  int x{-1}, y{-1};
  constexpr inline Point() noexcept = default;

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
  }

  constexpr inline Point& operator-=(Dir dir) noexcept {
    return *this -= Point{dir};
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

  [[nodiscard]] constexpr inline auto operator<=>(Point const& rhs) const noexcept = default;

  [[nodiscard]] constexpr inline bool operator==(Point const& rhs) const noexcept = default;

  [[nodiscard]] constexpr inline std::size_t Index(std::integral auto dim) const noexcept {
    return static_cast<std::size_t>(dim) * static_cast<std::size_t>(y) + static_cast<std::size_t>(x);
  }
};
