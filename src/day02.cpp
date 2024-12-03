module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

export module day02;

export using Day02ParsedType = std::vector<std::vector<long>>;
export using Day02AnswerType = long;

export Day02ParsedType Day02Parse(std::string_view input) noexcept {
  return input | std::views::split('\n') | std::views::filter([](auto&& line) { return not line.empty(); }) |
         std::views::transform([](auto&& line) {
           return line | std::views::split(' ') | std::views::transform([](auto&& num) -> long {
                    long v{0};
                    std::ignore = std::from_chars(num.begin(), num.end(), v);
                    return v;
                  }) |
                  std::ranges::to<std::vector>();
         }) |
         std::ranges::to<std::vector>();
}

bool Day02CheckLevel(std::vector<long> const& level) {
  std::ranges::random_access_range auto diffs = std::views::adjacent_transform<2>(level, std::minus{});
  return std::ranges::fold_left(diffs, (diffs.front() != 0), [pos{diffs.front() > 0}](bool valid, long diff) {
    return valid and (pos ? (1 <= diff and diff <= 3) : (-3 <= diff and diff <= -1));
  });
}

bool Day02CheckLevelTolerant(std::vector<long>& temp, std::vector<long> const& level) {
  // populate with all-but-first value
  temp.assign(level.begin() + 1, level.end());
  return std::ranges::any_of(std::views::iota(0, static_cast<int>(level.size())), [&](int i) {
    if (i > 0) {
      // only need to reassign a single element (replace new removed value with the one removed prior)
      *std::next(temp.begin(), i - 1) = *std::next(level.begin(), i - 1);
    }
    return Day02CheckLevel(temp);
  });
}

export Day02AnswerType Day02Part1(Day02ParsedType const& data) {
  return std::ranges::count_if(data, Day02CheckLevel);
}

export Day02AnswerType Day02Part2(Day02ParsedType const& data,
                                  [[maybe_unused]] Day02AnswerType const& answer) {
  // let's at least pretend we care about performance by reducing (re)allocations
  static std::vector<long> curr;
  return std::ranges::count_if(data, std::bind_front(Day02CheckLevelTolerant, curr));
}
