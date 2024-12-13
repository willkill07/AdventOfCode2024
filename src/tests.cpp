#include <catch2/catch_test_macros.hpp>

import day01;

TEST_CASE("Day01", "[day][01]") {
  std::string_view input = R"(3   4
4   3
2   5
1   3
3   9
3   3
)";
  auto parsed = Day01Parse(input);
  auto part1 = Day01Part1(parsed);
  auto part2 = Day01Part2(parsed, part1);
  REQUIRE(part1 == 11);
  REQUIRE(part2 == 31);
}

import day02;

TEST_CASE("Day02", "[day][02]") {
  std::string_view input = R"(7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
)";
  auto parsed = Day02Parse(input);
  auto part1 = Day02Part1(parsed);
  auto part2 = Day02Part2(parsed, part1);
  REQUIRE(part1 == 2);
  REQUIRE(part2 == 4);
}

import day03;

TEST_CASE("Day03", "[day][03]") {
  std::string_view input = R"(xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))
)";
  auto parsed = Day03Parse(input);
  auto part1 = Day03Part1(parsed);
  auto part2 = Day03Part2(parsed, part1);
  REQUIRE(part1 == 161);
  REQUIRE(part2 == 48);
}

import day04;

TEST_CASE("Day04", "[day][04]") {
  std::string_view input = R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX
)";
  auto parsed = Day04Parse(input);
  auto part1 = Day04Part1(parsed);
  auto part2 = Day04Part2(parsed, part1);
  REQUIRE(part1 == 18);
  REQUIRE(part2 == 9);
}

import day05;

TEST_CASE("Day05", "[day][05]") {
  std::string_view input = R"(47|53
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
)";
  auto parsed = Day05Parse(input);
  auto part1 = Day05Part1(parsed);
  auto part2 = Day05Part2(parsed, part1);
  REQUIRE(part1 == 143);
  REQUIRE(part2 == 123);
}

import day06;

TEST_CASE("Day06", "[day][06]") {
  std::string_view input = R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
)";
  auto parsed = Day06Parse(input);
  auto part1 = Day06Part1(parsed);
  auto part2 = Day06Part2(parsed, part1);
  REQUIRE(part1 == 41);
  REQUIRE(part2 == 6);
}

import day07;

TEST_CASE("Day07", "[day][07]") {
  std::string_view input = R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20
)";
  auto parsed = Day07Parse(input);
  auto part1 = Day07Part1(parsed);
  auto part2 = Day07Part2(parsed, part1);
  REQUIRE(part1 == 3749);
  REQUIRE(part2 == 11387);
}

import day08;

TEST_CASE("Day08", "[day][08]") {
  std::string_view input = R"(............
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
)";
  auto parsed = Day08Parse(input);
  auto part1 = Day08Part1(parsed);
  auto part2 = Day08Part2(parsed, part1);
  REQUIRE(part1 == 14);
  REQUIRE(part2 == 34);
}

import day09;

TEST_CASE("Day09", "[day][09]") {
  std::string_view input = R"(2333133121414131402
)";
  auto parsed = Day09Parse(input);
  auto part1 = Day09Part1(parsed);
  auto part2 = Day09Part2(parsed, part1);
  REQUIRE(part1 == 1928);
  REQUIRE(part2 == 2858);
}

import day10;

TEST_CASE("Day10", "[day][10]") {
  std::string_view input = R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732
)";
  auto parsed = Day10Parse(input);
  auto part1 = Day10Part1(parsed);
  auto part2 = Day10Part2(parsed, part1);
  REQUIRE(part1 == 36);
  REQUIRE(part2 == 81);
}

import day11;

TEST_CASE("Day11", "[day][11]") {
  std::string_view input = R"(1 2024 1 0 9 9 2021976
)";
  auto parsed = Day11Parse(input);
  auto part1 = Day11Part1(parsed);
  auto part2 = Day11Part2(parsed, part1);
  REQUIRE(part1 == 55'312);
  REQUIRE(part2 == 226'596'360'258'785L);
}

import day12;

TEST_CASE("Day12", "[day][12]") {
  std::string_view input = R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE
)";
  auto parsed = Day12Parse(input);
  auto part1 = Day12Part1(parsed);
  auto part2 = Day12Part2(parsed, part1);
  REQUIRE(part1 == 1930);
  REQUIRE(part2 == 1206);
}

import day13;

TEST_CASE("Day13", "[day][13]") {
  std::string_view input = R"(Button A: X+94, Y+34
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
)";
  auto parsed = Day13Parse(input);
  auto part1 = Day13Part1(parsed);
  auto part2 = Day13Part2(parsed, part1);
  REQUIRE(part1 == 480);
  REQUIRE(part2 == 875'318'608'908L);
}
