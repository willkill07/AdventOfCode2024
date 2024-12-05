module;

#include <string_view>

export module day04;

export using Day04ParsedType = std::string_view;
export using Day04AnswerType = long;

export Day04ParsedType Day04Parse(std::string_view input) noexcept {
  return input;
}

constexpr std::string_view XMAS{"XMAS"};
constexpr std::string_view SAMX{"SAMX"};
constexpr long N{4};

constexpr auto CheckForXMASFrom = [](std::string_view data, std::size_t offset, std::size_t delta) {
  auto check = [&](std::string_view s) {
    for (std::size_t i = 0, o = offset; i < s.size(); ++i, o += delta) {
      if (data[o] != s[i]) {
        return false;
      }
    }
    return true;
  };
  return check(XMAS) or check(SAMX);
};

export Day04AnswerType Day04Part1(Day04ParsedType const& data) {
  long count{0};
  std::size_t const x_count{data.find('\n')};
  std::size_t const w(x_count + 1);
  std::size_t const y_count{data.size() / w};
  for (std::size_t y = 0; y < y_count; ++y) {
    bool const do_y{y + N <= y_count};
    for (std::size_t x = 0; x < x_count; ++x) {
      bool const do_x{x + N <= x_count};
      std::size_t const offset{y * w + x};
      count += // horizonal
          (do_x and CheckForXMASFrom(data, offset, 1)) +
          // vertical
          (do_y and CheckForXMASFrom(data, offset, w)) +
          // top-left to bottom-right
          (do_y and do_x and CheckForXMASFrom(data, offset, w + 1)) +
          // top-right to bottom-left
          (do_y and do_x and CheckForXMASFrom(data, offset + N - 1, w - 1));
    }
  }
  return count;
}

export Day04AnswerType Day04Part2(Day04ParsedType const& data,
                                  [[maybe_unused]] Day04AnswerType const& answer) {
  long count{0};
  std::size_t const x_count{data.find('\n')};
  std::size_t const w(x_count + 1);
  std::size_t const y_count{data.size() / w};
  for (std::size_t y = 1; y < y_count - 1; ++y) {
    for (std::size_t x = 1; x < x_count - 1; ++x) {
      std::size_t const offset{y * w + x};
      count += (data[offset] == 'A' and // center is A and diagonals must be SAM/MAS
                (data[offset - (w + 1)] + data[offset + (w + 1)]) == ('S' + 'M') and
                (data[offset - (w - 1)] + data[offset + (w - 1)]) == ('S' + 'M'));
    }
  }
  return count;
}
