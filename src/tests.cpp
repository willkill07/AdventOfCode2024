#include <string_view>

#include <catch2/catch_test_macros.hpp>

using std::string_view_literals::operator""sv;

import util;
import day01;
import day02;
import day03;
import day04;
import day05;
import day06;
import day07;
import day08;
import day09;
import day10;
import day11;
import day12;
import day13;
import day14;

template <typename Part1Answer, typename Part2Answer> struct Data {
  constexpr Data(Part1Answer p1, Part2Answer p2, std::string_view input) noexcept
      : part1{std::move(p1)}, part2{std::move(p2)}, input{input} {
  }
  Part1Answer part1;
  Part2Answer part2;
  std::string_view input;
};

template <typename Part1Answer, typename Part2Answer>
Data(Part1Answer, Part2Answer, std::string_view) -> Data<Part1Answer, Part2Answer>;

constexpr auto Day01Data = Data{11, 31, R"(3   4
4   3
2   5
1   3
3   9
3   3
)"sv};

constexpr auto Day02Data = Data{2, 4, R"(7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
)"sv};

constexpr auto Day03Data = Data{
    161,
    48,
    R"(xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))
)",
};

constexpr auto Day04Data = Data{18, 9, R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX
)"};

constexpr auto Day05Data = Data{143, 123, R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47
)"};

constexpr auto Day06Data = Data{41, 6, R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
)"};

constexpr auto Day07Data = Data{3749, 11387, R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20
)"};

constexpr auto Day08Data = Data{14, 34, R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............
)"};

constexpr auto Day09Data = Data{1928, 2858, R"(2333133121414131402
)"};

constexpr auto Day10Data = Data{36, 81, R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732
)"};

constexpr auto Day11Data = Data{55'312, 226'596'360'258'785L, R"(1 2024 1 0 9 9 2021976
)"};

constexpr auto Day12Data = Data{1930, 1206, R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE
)"};

constexpr auto Day13Data = Data{480, 875'318'608'908L, R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279
)"};

#define EMIT_TEST(DayNum)                               \
  TEST_CASE(#DayNum) {                                  \
    auto parsed = DayNum##Parse(DayNum##Data.input);    \
    auto part1 = DayNum##Part1(parsed);                 \
    auto part2 = DayNum##Part2(parsed, part1);          \
    REQUIRE(part1 == DayNum##Data.part1);               \
    if (#DayNum == "Day13"sv or #DayNum == "Day14"sv) { \
      /* No part 2 to test */                           \
    } else {                                            \
      REQUIRE(part2 == DayNum##Data.part2);             \
    }                                                   \
  }

EMIT_TEST(Day01)
EMIT_TEST(Day02)
EMIT_TEST(Day03)
EMIT_TEST(Day04)
EMIT_TEST(Day05)
EMIT_TEST(Day06)
EMIT_TEST(Day07)
EMIT_TEST(Day08)
EMIT_TEST(Day09)
EMIT_TEST(Day10)
EMIT_TEST(Day11)
EMIT_TEST(Day12)
EMIT_TEST(Day13)
// no tests for 14 due to hard-coded sizes with no way to detect under test
