
module;

#include <algorithm>
#include <ranges>
#include <string_view>
#include <vector>

#include <ankerl/unordered_dense.h>
#include <ctre.hpp>

export module day11;

namespace {

struct Target {
  unsigned a{0xFFFFFFFF};
  unsigned b{0xFFFFFFFF};
};

struct ParsedData {
  std::vector<Target> edges;
  std::vector<long> counts;
};

static constexpr auto Digits = [](long value) {
  long digits{1};
  for (long num{10}; value >= num; num *= 10) {
    ++digits;
  }
  return digits;
};

static constexpr auto Split = [](long value, long digits) {
  long divisor{10};
  for (long i = 1; i < digits; ++i) {
    divisor *= 10;
  }
  return std::div(value, divisor);
};

struct Data {
  std::size_t index{0};
  long first;
  long second;
};

static void BuildMapping(ankerl::unordered_dense::map<long, Data>& mapping, long n) {
  if (mapping.contains(n)) {
    return;
  }
  if (n == 0) {
    mapping[n] = Data{mapping.size(), 1, -1};
    BuildMapping(mapping, 1);
  } else if (long const digits = Digits(n); (digits & 1) == 0) {
    auto [q, r] = Split(n, digits / 2);
    mapping[n] = Data{mapping.size(), q, r};
    BuildMapping(mapping, q);
    BuildMapping(mapping, r);
  } else {
    mapping[n] = Data{mapping.size(), n * 2024, -1};
    BuildMapping(mapping, n * 2024);
  }
}

} // namespace

export using Day11ParsedType = ParsedData;
export using Day11AnswerType = long;

export Day11ParsedType Day11Parse(std::string_view input) noexcept {
  // grab numbers
  auto nums = ctre::search_all<"\\d+">(input) |
              std::views::transform([](auto&& match) { return match.template to_number<long>(); }) |
              std::ranges::to<std::vector>();
  // construct sparse mapping of values
  ankerl::unordered_dense::map<long, Data> mapping;
  for (long n : nums) {
    BuildMapping(mapping, n);
  }
  // convert the mapping to dense (zero-based) edge list
  std::vector<Target> edges(mapping.size());
  for (auto const& [n, v] : mapping) {
    auto const& [i, v0, v1] = v;
    edges[i].a = static_cast<unsigned>(mapping[v0].index);
    if (v1 != -1) {
      edges[i].b = static_cast<unsigned>(mapping[v1].index);
    }
  }
  // determine initial counts
  std::vector<long> counts(mapping.size(), 0L);
  for (long n : nums) {
    ++counts[mapping[n].index];
  }
  // yield results
  return ParsedData{.edges = std::move(edges), .counts = std::move(counts)};
}

static void
Step(std::vector<Target> const& edges, std::vector<long>& counts_in, std::vector<long>& counts_out) {
  for (auto [i, count] : std::views::enumerate(counts_in)) {
    Target const& out = edges[static_cast<unsigned>(i)];
    counts_out[out.a] += count;
    if (out.b != 0xFFFFFFFF) {
      counts_out[out.b] += count;
    }
    count = 0;
  }
}

export Day11AnswerType Day11Part1(Day11ParsedType const& data) noexcept {
  std::vector counts{data.counts};
  std::vector empty(counts.size(), 0L);
  for (int i = 0; i < 25; ++i) {
    Step(data.edges, counts, empty);
    std::swap(counts, empty);
  }
  return std::ranges::fold_left(counts, 0L, std::plus{});
}

export Day11AnswerType Day11Part2(Day11ParsedType const& data,
                                  [[maybe_unused]] Day11AnswerType const& answer) {
  std::vector counts{data.counts};
  std::vector empty(counts.size(), 0L);
  for (int i = 0; i < 75; ++i) {
    Step(data.edges, counts, empty);
    std::swap(counts, empty);
  }
  return std::ranges::fold_left(counts, 0L, std::plus{});
}
