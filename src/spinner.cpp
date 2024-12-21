module;

#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <print>
#include <random>
#include <ranges>
#include <thread>
#include <utility>

export module spinner;

export enum class AnswerType : unsigned { Part1, Part2 };

export enum class TimeType : unsigned { File, Parse, Part1, Part2, Total };

namespace {

constexpr unsigned Column(TimeType type) noexcept {
  switch (type) {
  case TimeType::File:
    return 48;
  case TimeType::Parse:
    return 57;
  case TimeType::Part1:
    return 66;
  case TimeType::Part2:
    return 75;
  case TimeType::Total:
    return 84;
  }
}

constexpr unsigned Column(AnswerType type) noexcept {
  switch (type) {
  case AnswerType::Part1:
    return 8;
  case AnswerType::Part2:
    return 28;
  }
}
constexpr std::string_view Format(TimeType) noexcept {
  return "{:>4}{:2s}";
}
constexpr std::string_view Format(AnswerType) noexcept {
  return "{: <17}";
}

using TimeArgs = std::tuple<long, std::string_view>;
using AnswerArgs = std::tuple<std::string_view>;

static auto const print = [](unsigned column, std::string_view format, auto const& variant) {
  std::visit(
      [&](auto const& arguments) {
        std::string const string = std::apply(
            [&]<typename... Args>(Args&&... args) {
              return std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
            },
            arguments);
        char buffer[128];
        std::ranges::fill(buffer, 0);
        auto out = std::format_to(buffer, "\r\033[{}C{}\r", column, string);
        write(0, buffer, static_cast<std::size_t>(out - buffer));
      },
      variant);
};

template <auto Lo, auto Hi>
  requires std::same_as<decltype(Lo), decltype(Hi)> and (Lo <= Hi)
struct SpinnerRange {
  using Type = std::remove_cvref_t<decltype(Lo)>;
  constexpr static Type lo = Lo;
  constexpr static Type hi = Hi;
  constexpr static unsigned size{Hi - Lo + 1};
  static_assert(sizeof(Type) <= 2, "Only char and char16_t are supported");
};

template <typename T>
concept IsSpinnerRange = requires {
  typename T::Type;
  { T::lo } -> std::convertible_to<typename T::Type>;
  { T::hi } -> std::convertible_to<typename T::Type>;
  sizeof(T::lo) == sizeof(typename T::Type);
  sizeof(T::hi) == sizeof(typename T::Type);
};

enum class Encoding { Ascii = 1, Unicode = 2 };

template <IsSpinnerRange Range> struct SpinnerText {
private:
  constexpr static std::size_t Count = (Range::hi - Range::lo) + 1;
  constexpr static Encoding TextEncoding = static_cast<Encoding>(sizeof(typename Range::Type));
  constexpr static unsigned Stride = (TextEncoding == Encoding::Ascii) ? 1 : 3;

  consteval static auto MakeTable() {
    std::array<char, Count * Stride> bytes;
    unsigned index{0};
    for (auto i = Range::lo; i <= Range::hi; ++i) {
      if constexpr (TextEncoding == Encoding::Ascii) {
        // ASCII -- just copy the character
        bytes[index++] = i;
      } else if constexpr (TextEncoding == Encoding::Unicode) {
        // Need to re-encode unicode characters :upside-down-smiling-face:
        constexpr char continuation(static_cast<char>(0b10000000));
        constexpr char three_byte_encode{static_cast<char>(0b11100000)};
        char x, y;
        if constexpr (std::endian::native == std::endian::little) {
          std::tie(y, x) = std::bit_cast<std::array<char, 2>>(i);
        } else {
          std::tie(x, y) = std::bit_cast<std::array<char, 2>>(i);
        }
        // 0b1110XXXX -- leading byte
        bytes[index++] = static_cast<char>(three_byte_encode | ((x & 0b11110000) >> 4));
        // 0b10XXXXYY -- payload byte
        bytes[index++] = static_cast<char>(continuation | ((x & 0b00001111) << 2) | ((y & 0b11000000) >> 6));
        // 0b10YYYYYY -- payload byte
        bytes[index++] = static_cast<char>(continuation | (y & 0b00111111));
      }
    }
    return bytes;
  }

  constexpr static auto table = MakeTable();
  constexpr static std::string_view text{table.data(), table.size()};

public:
  constexpr static std::string_view Get(std::integral auto index) {
    return text.substr(index * Stride, Stride);
  }

  constexpr static std::size_t Size() {
    return text.size() / Stride;
  }
};

template <unsigned Count> struct RandomOrder {
  static void Iterate(unsigned& curr) {
    static std::minstd_rand rng{0};
    curr = rng() % Count;
  }
};

template <unsigned Count> struct Cycle {
  static void Iterate(unsigned& curr) {
    if (++curr == Count) {
      curr = 0;
    }
  }
};

template <unsigned Count> struct PingPong {
  static void Iterate(unsigned& curr) {
    static bool increasing{true};
    if (increasing and curr + 1 == Count) {
      increasing = false;
    }
    if (not increasing and curr == 0) {
      increasing = true;
    }
    if (increasing) {
      ++curr;
    } else {
      --curr;
    }
  }
};

using SpinnerType = SpinnerText<SpinnerRange<u'⠀', u'⣿'>>;
using MovementType = RandomOrder<SpinnerType::Size()>;
constexpr static unsigned Width{6};

static constexpr auto PrintSpinner = [](unsigned column) {
  static unsigned index{0};
  char buffer[64];
  std::ranges::fill(buffer, 0);
  auto out = std::format_to(buffer, "\r\033[{}C", column);
  for (auto _ : std::views::iota(0u, Width)) {
    MovementType::Iterate(index);
    out = std::format_to(out, "{}", SpinnerType::Get(index));
  }
  *out++ = '\r';
  write(0, buffer, static_cast<std::size_t>(out - buffer));
};

} // namespace

export class Spinner {
  bool has_tty_;
  std::atomic<bool> enabled_{false};
  std::atomic<unsigned> offset_{0};
  std::mutex mutex_;
  std::condition_variable_any cv_;
  std::jthread thread_;
  std::optional<std::tuple<unsigned, std::string_view, std::variant<TimeArgs, AnswerArgs>>> data_{
      std::nullopt};

  void Run(std::stop_token const& token) {
    if (not has_tty_) {
      return;
    }
    std::unique_lock lock{mutex_};
    while (not token.stop_requested()) {
      cv_.wait_for(lock, token, std::chrono::milliseconds{50}, [&]() { return data_.has_value(); });
      if (data_.has_value()) {
        std::apply(print, std::move(*data_));
        data_.reset();
        cv_.notify_all();
      } else if (enabled_.load()) {
        PrintSpinner(offset_.load());
      }
    }
  }

public:
  Spinner(bool has_tty) : has_tty_{has_tty}, thread_{std::bind_front(&Spinner::Run, this)} {
  }

  void PutTime(TimeType type, long value, std::string_view units) {
    if (not has_tty_) {
      return;
    }
    std::unique_lock lock{mutex_};
    cv_.wait(lock, [&] { return not data_.has_value(); });
    data_.emplace(Column(type), Format(type), std::tuple{value, units});
    cv_.notify_one();
  }

  void PutAnswer(AnswerType type, std::string_view string) {
    if (not has_tty_) {
      return;
    }
    std::unique_lock lock{mutex_};
    cv_.wait(lock, [&] { return not data_.has_value(); });
    data_.emplace(Column(type), Format(type), std::tuple{string});
    cv_.notify_one();
  }

  void Sync() {
    if (not has_tty_) {
      return;
    }
    std::unique_lock lock{mutex_};
    cv_.wait(lock, [&]() { return not data_.has_value(); });
  }

  void SetLocation(TimeType type) {
    if (not has_tty_) {
      return;
    }
    offset_.store(Column(type));
  }

  void SetLocation(AnswerType type) {
    if (not has_tty_) {
      return;
    }
    offset_.store(Column(type));
  }

  [[nodiscard]] bool HasTTY() const noexcept {
    return has_tty_;
  }

  void Enable() {
    if (not has_tty_) {
      return;
    }
    enabled_.store(true);
  }

  void Disable() {
    if (not has_tty_) {
      return;
    }
    enabled_.store(false);
  }
};
