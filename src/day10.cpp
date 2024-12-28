module;

#include <atomic>
#include <string_view>
#include <vector>

#include <ankerl/unordered_dense.h>

#include "circular_buffer.hpp"
#include "hashing.hpp"
#include "point.hpp"

export module day10;

import threading;

struct GridWithInfo {
  std::string_view grid;
  int dim;
  std::vector<Point> trailheads;
};

export using Day10ParsedType = GridWithInfo;
export using Day10AnswerType = long;

export GridWithInfo Day10Parse(std::string_view input) noexcept {
  int const dim{static_cast<int>(input.find('\n'))};
  std::vector<Point> peaks, trailheads;
  peaks.reserve(250);
  trailheads.reserve(300);
  for (Point loc{0, 0}; char c : input) {
    if (c == '\n') {
      ++loc.y;
      loc.x = 0;
      continue;
    } else if (c == '0') {
      trailheads.push_back(loc);
    }
    ++loc.x;
  }
  return GridWithInfo{.grid = input, .dim = dim, .trailheads = std::move(trailheads)};
}

static constexpr auto TransparentInsert = [](auto& container, auto const& value) {
  if constexpr (requires { container.push_back(value); }) {
    container.push_back(value);
  } else if constexpr (requires { container.insert(value); }) {
    container.insert(value);
  } else if constexpr (requires { container.push(value); }) {
    container.push(value);
  } else {
    static_assert(false, "Unable to insert");
  }
};

template <typename Container>
void TraverseFrom(GridWithInfo const& data, std::atomic_long& result, Point const& p) {
  auto grid = [&data](Point const& p) noexcept { return data.grid[p.Index(data.dim + 1)]; };
  thread_local CircularBuffer<Point> queue{64};
  thread_local Container seen;
  queue.clear();
  seen.clear();
  queue.push_back(p);
  while (not queue.empty()) {
    Point const curr{queue.back()};
    queue.pop_back();
    if (char const curr_height{grid(curr)}; curr_height == '9') {
      TransparentInsert(seen, curr);
    } else {
#pragma unroll
      for (Dir const dir : {Dir::Up, Dir::Down, Dir::Left, Dir::Right}) {
        if (Point const next{curr + dir};           //
            (0 <= next.x and next.x < data.dim) and //
            (0 <= next.y and next.y < data.dim) and //
            grid(next) == curr_height + 1) {
          queue.push_back(next);
        }
      }
    }
  }
  result += static_cast<int>(seen.size());
}

export Day10AnswerType Day10Part1(Day10ParsedType const& data) noexcept {
  std::atomic_long result{0};
  threading::ParallelForEach(data.trailheads,
                             std::bind_front(TraverseFrom<ankerl::unordered_dense::set<Point, Hash>>,
                                             std::cref(data),
                                             std::ref(result)),
                             threading::GetNumThreads() / 2);
  return result;
}

export Day10AnswerType Day10Part2(Day10ParsedType const& data,
                                  [[maybe_unused]] Day10AnswerType const& answer) {
  std::atomic_long result{0};
  threading::ParallelForEach(
      data.trailheads,
      std::bind_front(TraverseFrom<std::vector<Point>>, std::cref(data), std::ref(result)),
      threading::GetNumThreads() / 2);
  return result;
}
