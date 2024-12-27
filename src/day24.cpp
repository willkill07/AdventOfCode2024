module;

#include <algorithm>
#include <mutex>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include <ankerl/unordered_dense.h>
#include <ctre.hpp>

export module day24;

import threading;

template <typename... Fns> struct Overloaded : Fns... {
  using Fns::operator()...;
};

struct BinOp {
  std::string_view op, src1, src2;
};

struct Circuit {
  ankerl::unordered_dense::map<std::string_view, std::variant<bool, BinOp>> wires;
  std::string_view max_z{"z00"};

  void AddWire(std::string_view name, bool value) {
    wires.emplace(name, value);
    if (name[0] == 'z') {
      max_z = std::max(name, max_z);
    }
  }

  void AddWire(std::string_view name, std::string_view op, std::string_view src1, std::string_view src2) {
    wires.emplace(name, BinOp{op, src1, src2});
    if (name[0] == 'z') {
      max_z = std::max(name, max_z);
    }
  }

  bool Evaluate(std::string_view name) {
    return std::visit( //
        Overloaded{    //
                   [](bool value) { return value; },
                   [this, name](BinOp const& bin_op) {
                     bool const s1{Evaluate(bin_op.src1)};
                     bool const s2{Evaluate(bin_op.src2)};
                     bool const result{[&]() -> bool {
                       if (bin_op.op == "AND") {
                         return s1 & s2;
                       } else if (bin_op.op == "OR") {
                         return s1 | s2;
                       } else {
                         return s1 ^ s2;
                       }
                     }()};
                     wires.at(name) = result;
                     return result;
                   }},
        wires.at(name));
  }

  unsigned long GetZ() {
    unsigned long result{0};
    std::array<char, 3> buffer;
    std::string_view view{buffer.begin(), buffer.end()};
    for (int i{0}; true; ++i) {
      std::format_to(buffer.begin(), "z{:02d}", i);
      if (not wires.contains(view)) {
        return result;
      }
      result = result | (static_cast<unsigned long>(Evaluate(view)) << i);
    }
  }

  [[nodiscard]] std::string_view MaxZ() const noexcept {
    return max_z;
  }
};

export using Day24ParsedType = Circuit;
export using Day24AnswerType = unsigned long;

export Day24ParsedType Day24Parse(std::string_view input) noexcept {
  Circuit circuit;
  std::size_t const split = input.find("\n\n");
  for (auto [_, var, val] : ctre::search_all<R"((\w+): (\d))">(input.substr(0, split))) {
    circuit.AddWire(var.view(), static_cast<bool>(val.view()[0] == '1'));
  }
  for (auto [_, src1, op, src2, dst] :
       ctre::search_all<R"((\w+) (\w+) (\w+) -> (\w+))">(input.substr(split))) {
    circuit.AddWire(dst.view(), op.view(), src1.view(), src2.view());
  }
  return circuit;
}

export unsigned long Day24Part1(Circuit circuit) noexcept {
  return circuit.GetZ();
}

export std::string Day24Part2(Circuit const& circuit, [[maybe_unused]] unsigned long part1_answer) {
  using std::string_view_literals::operator""sv;
  std::vector<std::string_view> wrong;
  wrong.reserve(8);
  auto ops = circuit.wires |
             std::views::filter([](auto&& w) { return std::holds_alternative<BinOp>(w.second); }) |
             std::views::transform([](auto&& w) { return std::pair{w.first, std::get<BinOp>(w.second)}; }) |
             std::ranges::to<std::vector>();
  std::mutex mutex;
  threading::ParallelForEach(ops, [&] (auto&& res_src) {
    auto&& [res, src] = res_src;
    if (auto const& [op, src1, src2] = src; op[0] == 'X') {
      if (res[0] < 'x' and src1[0] < 'x' and src2[0] < 'x') {
        std::lock_guard _{mutex};
        wrong.push_back(res);
      } else {
        for (auto&& [nres, nsrc] : ops) {
          if (auto const& [nop, nsrc1, nsrc2] = nsrc; (res == nsrc1 or res == nsrc2) and nop[0] == 'O') {
            std::lock_guard _{mutex};
            wrong.push_back(res);
          }
        }
      }
    } else if (res[0] == 'z' and res != circuit.MaxZ()) {
      std::lock_guard _{mutex};
      wrong.push_back(res);
    } else if (op[0] == 'A' and src1 != "x00"sv and src2 != "x00"sv) {
      for (auto&& [nres, nsrc] : ops) {
        if (auto const& [nop, nsrc1, nsrc2] = nsrc; (res == nsrc1 or res == nsrc2) and nop[0] != 'O') {
          std::lock_guard _{mutex};
          wrong.push_back(res);
        }
      }
    }
  }, /*threads=*/4);
  std::ranges::sort(wrong);
  return std::ranges::to<std::string>(std::views::join_with(wrong, ','));
}
