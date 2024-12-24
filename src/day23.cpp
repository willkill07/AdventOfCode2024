module;

#include <algorithm>
#include <array>
#include <ranges>
#include <string_view>
#include <vector>

#include <ankerl/unordered_dense.h>
#include <ctre.hpp>

export module day23;

inline static constexpr std::size_t N{26};

namespace {

struct Graph {
  ankerl::unordered_dense::map<unsigned, std::vector<unsigned>> nodes;
  std::array<std::array<bool, N * N>, N * N> edges;
};

inline constexpr unsigned ToIndex(char c) noexcept {
  return static_cast<unsigned>(c - 'a');
}

inline constexpr char ToChar(unsigned n) noexcept {
  return static_cast<char>('a' + n);
}

inline constexpr unsigned ToId(std::string_view s) {
  return N * ToIndex(s[0]) + ToIndex(s[1]);
}

} // namespace

export Graph Day23Parse(std::string_view input) noexcept {
  ankerl::unordered_dense::map<unsigned, std::vector<unsigned>> nodes;
  nodes.reserve(N * N);
  std::array<std::array<bool, N * N>, N * N> adj;
  for (auto& chunk : adj) {
    chunk.fill(false);
  }
  for (auto [_, a, b] : ctre::multiline_search_all<R"(^(..)-(..)\n)">(input)) {
    unsigned const node_a{ToId(a.view())};
    unsigned const node_b{ToId(b.view())};
    adj[node_a][node_b] = adj[node_b][node_a] = true;
    auto& a_list = nodes.try_emplace(node_a, std::vector<unsigned>{}).first->second;
    a_list.reserve(16);
    a_list.push_back(node_b);
    auto& b_list = nodes.try_emplace(node_b, std::vector<unsigned>{}).first->second;
    b_list.reserve(16);
    b_list.push_back(node_a);
  }
  return Graph{.nodes = std::move(nodes), .edges = adj};
}

export unsigned Day23Part1(Graph const& data) noexcept {
  auto const& [nodes, edges] = data;
  std::array<bool, N * N> seen;
  seen.fill(false);
  unsigned triangles{0};
  for (unsigned n1 : std::views::iota(494U, 520U)) {
    if (auto iter = nodes.find(n1); iter != nodes.end()) {
      seen[n1] = true;
      for (auto [i, n2] : std::views::enumerate(iter->second)) {
        if (seen[n2]) {
          continue;
        }
        for (auto n3 : std::views::drop(iter->second, i)) {
          if (not seen[n3] and edges[n2][n3]) {
            ++triangles;
          }
        }
      }
    }
  }
  return triangles;
}

export std::string Day23Part2(Graph const& data, [[maybe_unused]] unsigned const& answer) {
  auto const& [nodes, edges] = data;
  std::array<bool, N * N> seen;
  seen.fill(false);
  std::vector<unsigned> clique, largest;
  for (auto const& [n1, neighbors] : nodes) {
    if (seen[n1]) {
      continue;
    }
    clique.clear();
    clique.push_back(n1);
    for (auto n2 : neighbors) {
      if (std::ranges::all_of(clique, [&](unsigned c) { return edges[n2][c]; })) {
        seen[n2] = true;
        clique.push_back(n2);
      }
      if (clique.size() > largest.size()) {
        largest = clique;
      }
    }
  }

  std::string result;
  result.reserve(3 * largest.size());
  std::ranges::sort(largest);
  for (unsigned n : largest) {
    result.push_back(ToChar(n / 26));
    result.push_back(ToChar(n % 26));
    result.push_back(',');
  }

  result.pop_back();
  return result;
}
