module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <print>

#include <ctre.hpp>

export module day17;

struct ComputerState {
  std::uint64_t instructions;
  std::uint64_t output{0};
  std::uint64_t a, b, c;
  std::uint8_t ilen{0}, olen{0};

  [[nodiscard]] inline std::uint64_t Combo(std::uint8_t v) const {
    switch (v) {
    case 4:
      return a;
    case 5:
      return b;
    case 6:
      return c;
    default:
      return v;
    }
  }

  [[nodiscard]] std::uint64_t Output() const noexcept {
    auto const bits{olen * 3U};
    std::uint64_t const mask{(1LU << bits) - 1};
    return output & mask;
  }

  [[nodiscard]] std::string OutputString() const noexcept {
    std::string str(2 * olen - 1, ',');
    for (auto out{Output()}; auto& o : std::views::stride(std::views::reverse(str), 2)) {
      o = static_cast<char>((out & 0x7) + '0');
      out >>= 3;
    }
    return str;
  }

  void Run() {
    auto ops = [&](auto i) -> std::uint8_t {
      auto discard = (ilen / 2) - 1 - (i / 2);
      return static_cast<std::uint8_t>((instructions >> (discard * 6)) & 0b111111);
    };
    output = 0;
    olen = 0;
    std::uint8_t pc{0};
    while (pc < ilen) {
      std::uint8_t const o{ops(pc)};
      std::uint8_t const op(o >> 3);
      std::uint8_t const arg(o & 0b111);
      switch (op) {
      case 0:
        a >>= Combo(arg);
        pc += 2;
        break;
      case 1:
        b ^= arg;
        pc += 2;
        break;
      case 2:
        b = Combo(arg) & 0x7;
        pc += 2;
        break;
      case 3:
        pc = (a == 0 ? pc + 2 : arg);
        break;
      case 4:
        b ^= c;
        pc += 2;
        break;
      case 5:
        output = (output << 3) | (Combo(arg) & 0x7);
        ++olen;
        pc += 2;
        break;
      case 6:
        b = a >> Combo(arg);
        pc += 2;
        break;
      case 7:
        c = a >> Combo(arg);
        pc += 2;
        break;
      default:
        __builtin_unreachable();
      }
    }
  }
};

export using Day17ParsedType = ComputerState;
export using Day17AnswerType = std::string;

export Day17ParsedType Day17Parse(std::string_view input) noexcept {
  auto [_, a, b, c, prog] = ctre::multiline_match<R"(Register A: (\d+)
Register B: (\d+)
Register C: (\d+)

Program: ([0-9,]+)
)">(input);
  std::uint64_t instr{0};
  std::uint8_t ilen{0};
  for (char o : std::views::stride(prog, 2)) {
    instr = (instr << 3) | static_cast<std::uint8_t>(o - '0');
    ++ilen;
  }
  return ComputerState{.instructions = instr,
                       .a = a.to_number<std::uint64_t>(),
                       .b = b.to_number<std::uint64_t>(),
                       .c = c.to_number<std::uint64_t>(),
                       .ilen = ilen};
}

export Day17AnswerType Day17Part1(ComputerState state) noexcept {
  state.Run();
  return state.OutputString();
}

[[nodiscard]] std::optional<std::uint64_t>
FindA(ComputerState const& original, std::size_t bits, std::uint64_t partial) {
  for (std::uint8_t c{0}; c < 8; ++c) {
    std::uint64_t const next{(partial << 3) | c};
    ComputerState state{original};
    state.a = next;
    state.Run();
    std::uint64_t const mask{(1LU << (3 * state.olen)) - 1};
    if ((original.instructions & mask) == state.Output()) {
      if (bits == state.ilen * 3LU) {
        return next;
      }
      if (auto res = FindA(original, bits + 3, next); res) {
        return res;
      }
    }
  }
  return std::nullopt;
}

export Day17AnswerType Day17Part2(ComputerState const& state,
                                  [[maybe_unused]] Day17AnswerType const& answer) {
  return std::to_string(*FindA(state, 3, 0));
}
