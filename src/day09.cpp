module;

#include <algorithm>
#include <functional>
#include <limits>
#include <ranges>
#include <string_view>
#include <utility>
#include <vector>

export module day09;

export using Day09ParsedType = std::vector<unsigned>;
export using Day09AnswerType = std::size_t;

export Day09ParsedType Day09Parse(std::string_view input) noexcept {
  input.remove_suffix(1);
  return std::ranges::to<std::vector>(
      std::views::transform(input, [](char c) { return static_cast<unsigned>(c - '0'); }));
}

static inline constexpr std::array TRIANGULAR{0U, 0U, 1U, 3U, 6U, 10U, 15U, 21U, 28U, 36U};

static inline void
UpdateChecksum(std::size_t& checksum, unsigned block, unsigned index, unsigned size) noexcept {
  checksum += static_cast<std::size_t>(index / 2) * (block * size + TRIANGULAR[size]);
}

export Day09AnswerType Day09Part1(Day09ParsedType const& disk) noexcept {
  unsigned free{0}, available{0}, needed{0}, block{0};
  std::size_t checksum{0};
  auto file = static_cast<unsigned>(disk.size() + (disk.size() & 1));
  while (free < file) {
    unsigned const size{std::min(needed, available)};
    UpdateChecksum(checksum, std::exchange(block, block + size), file, size);
    if ((needed -= size) == 0) {
      needed = disk[file -= 2];
    }
    if ((available -= size) == 0) {
      available = disk[free + 1];
      UpdateChecksum(
          checksum, std::exchange(block, block + disk[free]), std::exchange(free, free + 2), disk[free]);
    }
  }
  UpdateChecksum(checksum, block, file, needed);
  return checksum;
}

export Day09AnswerType Day09Part2(Day09ParsedType const& disk, [[maybe_unused]] Day09AnswerType const& answer) {
  std::vector<std::vector<unsigned>> free(10);
  for (auto& f : free) {
    f.reserve(1'024);
  }
  std::size_t checksum{0};
  unsigned block{
      std::ranges::fold_left(std::views::enumerate(disk), 0U, [&](unsigned block, auto const& pair) {
        auto const& [index, size] = pair;
        if ((index & 1) == 1 and size > 0) {
          free[size].push_back(block);
          std::ranges::push_heap(free[size], std::greater{});
        }
        return block + size;
      })};
  for (auto&& [index, size] : std::views::reverse(std::views::enumerate(disk))) {
    block -= size;
    if ((index & 1) == 1) {
      continue;
    }
    // Only consider blocks with data
    unsigned next_block{block}, next_index{std::numeric_limits<unsigned>::max()};
    for (auto const& [i, heap] : std::views::drop(std::views::enumerate(free), size)) {
      if (not heap.empty()) {
        if (auto const first = heap.front(); first < next_block) {
          std::tie(next_block, next_index) = std::tuple{first, static_cast<unsigned>(i)};
        }
      }
    }
    // Drop larger blocks if all free blocks are to the right of where we currently are
    if (not free.empty()) {
      if (auto last = static_cast<unsigned>(free.size() - 1); not free[last].empty()) {
        if (auto first = free[last].front(); first > block) {
          free.pop_back();
        }
      }
    }
    // Update free blocks
    if (next_index != std::numeric_limits<unsigned>::max()) {
      std::ranges::pop_heap(free[next_index], std::greater{});
      free[next_index].pop_back();
      if (size < next_index) {
        free[next_index - size].push_back(next_block + size);
        std::ranges::push_heap(free[next_index - size], std::greater{});
      }
    }
    // Update the checksum with the file's location (possibly unchanged).
    checksum += static_cast<std::size_t>(index / 2) * (next_block * size + TRIANGULAR[size]);
  }
  return checksum;
}
