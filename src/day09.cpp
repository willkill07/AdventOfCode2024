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
    UpdateChecksum(checksum, block, file, size);
    block += size;
    needed -= size;
    available -= size;
    if (needed == 0) {
      needed = disk[file -= 2];
    }
    if (available == 0) {
      available = disk[free + 1];
      UpdateChecksum(checksum, block, free, disk[free]);
      block += disk[free];
      free += 2;
    }
  }
  UpdateChecksum(checksum, block, file, needed);
  return checksum;
}

static auto MakeFreeBlockMap(std::vector<unsigned> const& disk) {
  std::vector<std::vector<unsigned>> free(10);
  for (auto& f : free) {
    f.reserve(1'024);
  }
  auto block_sorter = [&](unsigned block, auto const& pair) {
    auto const& [index, size] = pair;
    if ((index & 1) == 1 and size > 0) {
      free[size].push_back(block);
    }
    return block + size;
  };
  unsigned block{std::ranges::fold_left(std::views::enumerate(disk), 0U, block_sorter)};
  std::ranges::for_each(free, [](auto& f) { std::ranges::make_heap(f, std::greater{}); });
  return std::pair{block, std::move(free)};
}

export Day09AnswerType Day09Part2(Day09ParsedType const& disk,
                                  [[maybe_unused]] Day09AnswerType const& answer) {
  auto [block, free] = MakeFreeBlockMap(disk);
  std::size_t checksum{0};
  for (auto [index, size] : std::views::reverse(std::views::enumerate(disk))) {
    block -= size;
    if ((index & 1) == 1) {
      continue;
    }
    // Find the candidate block to use
    auto const [block_id, block_size] =
        std::ranges::fold_left(std::views::drop(std::views::enumerate(free), size),
                               std::pair{block, std::numeric_limits<unsigned>::max()},
                               [](auto&& next, auto&& curr) {
                                 return std::apply([&](long i, auto& heap) {
                                    if (not heap.empty()) {
                                      if (auto const first = heap.front(); first < next.first) {
                                        return std::pair{first, static_cast<unsigned>(i)};
                                      }
                                    }
                                    return next;
                                  }, curr);
                               });
    // Drop larger blocks if all free blocks are to the right of where we currently are
    if (not free.empty()) {
      if (auto last = static_cast<unsigned>(free.size() - 1); not free[last].empty()) {
        if (auto first = free[last].front(); first > block) {
          free.pop_back();
        }
      }
    }
    // Update block
    if (block_size != std::numeric_limits<unsigned>::max()) {
      std::ranges::pop_heap(free[block_size], std::greater{});
      free[block_size].pop_back();
      if (size < block_size) {
        free[block_size - size].push_back(block_id + size);
        std::ranges::push_heap(free[block_size - size], std::greater{});
      }
    }
    // Update the checksum with the file's location (possibly unchanged).
    UpdateChecksum(checksum, block_id, static_cast<unsigned>(index), size);
  }
  return checksum;
}
