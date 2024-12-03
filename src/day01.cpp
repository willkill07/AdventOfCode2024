module;

#include <algorithm>
#include <charconv>
#include <concepts>
#include <ranges>
#include <string_view>
#include <vector>

export module day01;

constexpr auto absdiff = []<typename T>(T const& x, std::same_as<T> auto const& y) noexcept {
  T const diff{x - y};
  return (diff < 0) ? -diff : diff;
};

export using Day01ParsedType = std::vector<std::pair<long, long>>;
export using Day01AnswerType = long;

export Day01ParsedType Day01Parse(std::string_view input) noexcept {
  Day01ParsedType res;
  res.reserve(1000);
  std::from_chars_result r{.ptr = input.data(), .ec = std::errc{}};
  char const* end{r.ptr + input.size()};
  while (r.ptr != end) {
    long f{0}, s{0};
    // parse the first number
    if (r = std::from_chars(r.ptr, end, f); r.ec != std::errc{})
      break;
    // consume all whitespace
    while (*++r.ptr == ' ')
      ;
    // parse the second number
    if (r = std::from_chars(r.ptr, end, s); r.ec != std::errc{})
      break;
    res.emplace_back(f, s);
    // increment consumes the newline character
    ++r.ptr;
  }
  std::ranges::sort(std::views::keys(res));
  std::ranges::sort(std::views::values(res));
  return res;
}

export Day01AnswerType Day01Part1(Day01ParsedType const& data) noexcept {
  return std::ranges::fold_left(
      std::views::zip_transform(absdiff, std::views::keys(data), std::views::values(data)), 0L, std::plus{});
}

export Day01AnswerType Day01Part2(Day01ParsedType const& data,
                                  [[maybe_unused]] Day01AnswerType const& answer) {
  std::ranges::random_access_range auto a = std::views::keys(data);
  std::ranges::random_access_range auto b = std::views::values(data);
  std::random_access_iterator auto ib = b.begin();
  auto compute = [&](long x) noexcept -> long {
    // advance b until it is strictly >= x
    while (ib != b.end() and *ib < x) {
      ++ib;
    }
    std::random_access_iterator auto ob = ib;
    while (ib != b.end() and *ib == x) {
      ++ib;
    }
    return x * std::ranges::distance(ob, ib);
  };
  return std::ranges::fold_left(std::views::transform(a, compute), 0L, std::plus{});
}
