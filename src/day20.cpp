module;

#include <limits>
#include <string_view>
#include <vector>

export module day20;

namespace {

static constexpr int const infinite{std::numeric_limits<int>::max()};

struct DistanceMap {
  std::vector<int> distances;
  int width;
  int height;
  int offset;
};

constexpr auto Abs = [](std::integral auto x) { return (x < 0 ? -x : x); };

constexpr auto Manhattan = []<typename T>(T const& x, std::same_as<T> auto const& y) noexcept {
  return Abs(x) + Abs(y);
};

} // namespace

export using Day20ParsedType = DistanceMap;
export using Day20AnswerType = std::size_t;

export Day20ParsedType Day20Parse(std::string_view input) noexcept {
  int const width{static_cast<int>(input.find('\n'))};
  int const offset{static_cast<int>(width) + 1};
  int const start{static_cast<int>(input.find('S'))};
  int const end{static_cast<int>(input.find('E'))};

  std::vector<int> dist(static_cast<std::size_t>(width * width), infinite);
  int curr_dist{0};
  int loc{start};
  while (loc != end) {
    dist[static_cast<unsigned>(loc)] = curr_dist++;
#pragma unroll
    for (int const delta : {1, -1, offset, -offset}) {
      int const new_loc{loc + delta};
      if (input[static_cast<unsigned>(new_loc)] != '#' and dist[static_cast<unsigned>(new_loc)] == infinite) {
        loc = new_loc;
        break;
      }
    }
  }
  dist[static_cast<unsigned>(loc)] = curr_dist;
  return DistanceMap{.distances = std::move(dist),
                     .width = width,
                     .height = static_cast<int>(input.size()) / offset,
                     .offset = offset};
}

template <int Limit> [[nodiscard]] std::size_t CountCheats(DistanceMap const& map) {
  auto const& [distances, width, height, offset] = map;
  auto index = [&](int x, int y) { return static_cast<std::size_t>(y * static_cast<long>(offset) + x); };
  std::size_t count{0};
  for (int y{1}; y < height - 1; ++y) {
    for (int x{1}; x < width - 1; ++x) {
      if (int const c{distances[index(x, y)]}; c != infinite) {
        for (int dy{0}; dy <= Limit; ++dy) {
          if (y + dy < height - 1) {
            for (int dx{dy == 0 ? 1 : dy - Limit}; dx <= Limit - dy; ++dx) {
              if (x + dx >= 1 and x + dx < width - 1) {
                if (int const n{distances[index(x + dx, y + dy)]}; n != infinite) {
                  int const manhattan{Manhattan(dx, dy)};
                  if (int const cheat{Abs(n - c) - manhattan}; cheat >= 100 and manhattan <= Limit) {
                    ++count;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return count;
}

export Day20AnswerType Day20Part1(Day20ParsedType const& data) noexcept {
  return CountCheats<2>(data);
}

export Day20AnswerType Day20Part2(Day20ParsedType const& data,
                                  [[maybe_unused]] Day20AnswerType const& answer) {
  return CountCheats<20>(data);
}
