module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

#include <ctre.hpp>

export module day02;

constexpr auto Valid = [](std::ranges::random_access_range auto vals) {
  auto diffs = std::views::adjacent_transform<2>(vals, std::minus{});
  return std::ranges::all_of(diffs, [negative{diffs.front() < 0}](char d) {
    char const val(negative ? -d : d);
    return (1 <= val and val <= 3);
  });
};

struct Values {
private:
  long count_{0};
  std::array<char, 8> values_{};

public:
  Values& Update(long num) {
    *(values_.data() + count_++) = static_cast<char>(num);
    return *this;
  }

  [[nodiscard]] bool Valid() const noexcept {
    return ::Valid(std::views::take(values_, count_));
  }

  [[nodiscard]] bool PotentiallyValid() const noexcept {
    std::array<char, 8> vals;
    std::ranges::copy(values_ | std::views::take(count_) | std::views::drop(1), vals.begin());
    auto [in, out] = std::pair{values_.begin(), vals.begin()};
    return std::ranges::any_of(std::views::iota(0, static_cast<int>(count_)), [&](int i) {
      if (i > 0) {
        *out++ = *in++;
      }
      return ::Valid(std::views::take(vals, count_ - 1));
    });
  }
};

export using Day02ParsedType = std::vector<Values>;
export using Day02AnswerType = long;

export Day02ParsedType Day02Parse(std::string_view input) noexcept {
  return std::ranges::to<std::vector>(std::views::transform(
      ctre::split<"\n">(input) | std::views::filter([](auto line) { return line.size() > 0; }),
      [](auto line) {
        return std::ranges::fold_left(
            std::views::transform(ctre::split<" ">(line),
                                  [](auto&& num) { return num.template to_number<long>(); }),
            Values{},
            &Values::Update);
      }));
}

export Day02AnswerType Day02Part1(Day02ParsedType const& data) {
  return std::ranges::count_if(data, &Values::Valid);
}

export Day02AnswerType Day02Part2(Day02ParsedType const& data,
                                  [[maybe_unused]] Day02AnswerType const& answer) {
  return std::ranges::count_if(data, &Values::PotentiallyValid);
}
