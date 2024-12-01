#include "day.hpp"

namespace day01 {
using parsed = std::vector<int>;
using part1_answer = int;
using part2_answer = int;

[[nodiscard]] [[gnu::noinline]] static parsed
do_parse(std::span<char const> bytes) {
  return {};
}

[[nodiscard]] [[gnu::noinline]] static part1_answer
do_part1(parsed const &data) {
  return {};
}

[[nodiscard]] [[gnu::noinline]] static part2_answer
do_part2(parsed const &data, [[maybe_unused]] part1_answer const &answer) {
  return {};
}

} // namespace day01

int main() {
  SOLVE_DAY(day01);
}
