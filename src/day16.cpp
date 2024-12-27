module;

#include <algorithm>
#include <array>
#include <limits>
#include <ranges>
#include <string_view>
#include <vector>

#include "circular_buffer.hpp"

export module day16;

namespace {

struct Data {
  unsigned pos;
  unsigned dir;
  unsigned cost;
};

template <typename T> constexpr T const max = std::numeric_limits<T>::max();

} // namespace

export using Day16ParsedType = std::pair<long, long>;
export using Day16AnswerType = long;

export Day16ParsedType Day16Parse(std::string_view maze) noexcept {
  int const offset{static_cast<int>(maze.find('\n') + 1)};
  unsigned const start{static_cast<unsigned>(maze.find('S'))};
  unsigned const end{static_cast<unsigned>(maze.find('E'))};
  std::array const dirs{1, offset, -1, -offset};
  CircularBuffer<Data> todo_first{768}, todo_second{768};
  std::vector seen(maze.size(), std::array{max<unsigned>, max<unsigned>, max<unsigned>, max<unsigned>});
  unsigned lowest{max<int>};
  todo_first.push_back(Data{start, 0, 0});
  seen[start][0] = 0;
  while (not todo_first.empty()) {
    while (not todo_first.empty()) {
      auto [pos, dir, cost] = todo_first.front();
      todo_first.pop_front();
      if (cost >= lowest) {
        continue;
      }
      if (pos == end) {
        lowest = cost;
        continue;
      }
      unsigned const left{(dir + 3) % 4}, right{(dir + 1) % 4};
      for (auto next : {
               Data(static_cast<unsigned>(static_cast<int>(pos) + dirs[dir]), dir, cost + 1),
               Data(pos, left, cost + 1000),
               Data(pos, right, cost + 1000),
           }) {
        if (maze[next.pos] != '#' && next.cost < seen[next.pos][next.dir]) {
          // Find the next bucket.
          (next.dir == dir ? todo_first : todo_second).push_back(next);
          seen[next.pos][next.dir] = next.cost;
        }
      }
    }
    std::swap(todo_first, todo_second);
  }
  CircularBuffer<Data> todo{32};
  std::vector visited(maze.size(), 0);
  for (auto dir : std::views::iota(0u, 4u)) {
    if (seen[end][dir] == lowest) {
      todo.push_back({end, dir, lowest});
    }
  }

  while (not todo.empty()) {
    auto [pos, dir, cost] = todo.front();
    todo.pop_front();
    visited[pos] = 1;
    if (pos == start) {
      continue;
    }
    unsigned const left{(dir + 3) % 4}, right{(dir + 1) % 4};
    for (Data const next : {
             Data{static_cast<unsigned>(static_cast<int>(pos) - dirs[dir]), dir, cost - 1},
             Data{pos, left, cost - 1000},
             Data{pos, right, cost - 1000},
         }) {
      if (next.cost == seen[next.pos][next.dir]) {
        todo.push_back(next);
        seen[next.pos][next.dir] = max<unsigned>;
      }
    }
  }
  return std::pair{lowest, static_cast<unsigned>(std::ranges::count(visited, 1))};
}

export Day16AnswerType Day16Part1(Day16ParsedType const& data) noexcept {
  return data.first;
}

export Day16AnswerType Day16Part2(Day16ParsedType const& data,
                                  [[maybe_unused]] Day16AnswerType const& answer) {

  return data.second;
}
