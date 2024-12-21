module;

#include <ranges>
#include <string_view>
#include <vector>


#include <print>

#include <ctre.hpp>

export module day19;

export using Day19ParsedType = std::pair<std::size_t, std::size_t>;
export using Day19AnswerType = std::size_t;

namespace {

constexpr std::uint32_t Hash(char c) {
  return ((c >> 4) ^ c) & 0x7;
}

struct Node {
  std::array<std::size_t, 6> next{0, 0, 0, 0, 0, 0};
  void SetTowel() {
    next[3] = 1;
  }
  [[nodiscard]] std::size_t Towels() const {
    return next[3];
  }
};

} // end namespace

export Day19ParsedType Day19Parse(std::string_view input) noexcept {
  std::size_t const split = input.find('\n');
  auto prefixes = ctre::split<", ">(input.substr(0, split)) |
                  std::views::transform([](auto&& match) { return match.view(); });
  auto words = ctre::split<"\n">(input.substr(split + 2)) |
               std::views::transform([](auto&& match) { return match.view(); });

  std::vector<Node> trie;
  trie.reserve(800);
  trie.emplace_back();

  for (std::string_view towel : prefixes) {
    if (towel.empty()) {
      continue;
    }
    std::size_t i{0};
    for (std::uint32_t j : std::views::transform(towel, Hash)) {
      if (trie[i].next[j] == 0) {
        // New prefix
        trie[i].next[j] = trie.size();
        i = trie.size();
        trie.emplace_back();
      } else {
        // Existing
        i = trie[i].next[j];
      }
    }
    trie[i].SetTowel();
  }

  std::size_t part_one{0}, part_two{0};
  std::vector<std::size_t> ways;
  ways.reserve(64);
  for (auto design : words) {
    std::size_t const size{design.size()};
    ways.clear();
    ways.resize(size + 1, 0);
    ways[0] = 1;
    for (std::size_t const start : std::views::iota(0LU, size)) {
      if (ways[start] > 0) {
        for (std::size_t i{0}; std::size_t const end : std::views::iota(start, size)) {
          // Follow link
          i = trie[i].next[Hash(design[end])];
          if (i == 0) {
            // Not valid
            break;
          }
          // Valid -- add ways
          ways[end + 1] += trie[i].Towels() * ways[start];
        }
      }
    }
    // Last element is the total possible combinations.
    std::size_t const total{ways[size]};
    part_one += (total > 0);
    part_two += total;
  }
  return std::pair{part_one, part_two};
}

export Day19AnswerType Day19Part1(Day19ParsedType const& data) noexcept {
  return data.first;
}

export Day19AnswerType Day19Part2(Day19ParsedType const& data,
                                  [[maybe_unused]] Day19AnswerType const& answer) {
  return data.second;
}
