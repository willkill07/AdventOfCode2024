module;

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <utility>

export module spinner;

export enum class AnswerType : unsigned { Part1, Part2 };

export enum class TimeType : unsigned { File, Parse, Part1, Part2, Total };

namespace {

unsigned Column(TimeType type) {
  switch (type) {
  case TimeType::File:
    return 44;
  case TimeType::Parse:
    return 53;
  case TimeType::Part1:
    return 62;
  case TimeType::Part2:
    return 71;
  case TimeType::Total:
    return 80;
  }
}

unsigned Column(AnswerType type) {
  switch (type) {
  case AnswerType::Part1:
    return 8;
  case AnswerType::Part2:
    return 26;
  }
}
std::string_view Format(TimeType) {
  return "{:>4}{:2s}";
}
std::string_view Format(AnswerType) {
  return "{:>15}";
}

} // namespace

static auto const print = [](unsigned column, std::string_view format, auto const& variant) {
  std::visit(
      [&](auto const& arguments) {
        std::string const string = std::apply(
            [&]<typename... Args>(Args&&... args) {
              return std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
            },
            arguments);
        (std::cout << "\r\033[" << column << 'C' << string << '\r').flush();
      },
      variant);
};

static constexpr auto PrintSpinner = [](unsigned column) {
  static std::size_t index{0};
  static std::minstd_rand rng{0};
  // TODO: programmatically generate
  constexpr static std::string_view bar{
      "\u2800\u2801\u2802\u2803\u2804\u2805\u2806\u2807\u2808\u2809\u280A\u280B\u280C\u280D\u280E\u280F"
      "\u2810\u2811\u2812\u2813\u2814\u2815\u2816\u2817\u2818\u2819\u281A\u281B\u281C\u281D\u281E\u281F"
      "\u2820\u2821\u2822\u2823\u2824\u2825\u2826\u2827\u2828\u2829\u282A\u282B\u282C\u282D\u282E\u282F"
      "\u2830\u2831\u2832\u2833\u2834\u2835\u2836\u2837\u2838\u2839\u283A\u283B\u283C\u283D\u283E\u283F"
      "\u2840\u2841\u2842\u2843\u2844\u2845\u2846\u2847\u2848\u2849\u284A\u284B\u284C\u284D\u284E\u284F"
      "\u2850\u2851\u2852\u2853\u2854\u2855\u2856\u2857\u2858\u2859\u285A\u285B\u285C\u285D\u285E\u285F"
      "\u2860\u2861\u2862\u2863\u2864\u2865\u2866\u2867\u2868\u2869\u286A\u286B\u286C\u286D\u286E\u286F"
      "\u2870\u2871\u2872\u2873\u2874\u2875\u2876\u2877\u2878\u2879\u287A\u287B\u287C\u287D\u287E\u287F"
      "\u2880\u2881\u2882\u2883\u2884\u2885\u2886\u2887\u2888\u2889\u288A\u288B\u288C\u288D\u288E\u288F"
      "\u2890\u2891\u2892\u2893\u2894\u2895\u2896\u2897\u2898\u2899\u289A\u289B\u289C\u289D\u289E\u289F"
      "\u28A0\u28A1\u28A2\u28A3\u28A4\u28A5\u28A6\u28A7\u28A8\u28A9\u28AA\u28AB\u28AC\u28AD\u28AE\u28AF"
      "\u28B0\u28B1\u28B2\u28B3\u28B4\u28B5\u28B6\u28B7\u28B8\u28B9\u28BA\u28BB\u28BC\u28BD\u28BE\u28BF"
      "\u28C0\u28C1\u28C2\u28C3\u28C4\u28C5\u28C6\u28C7\u28C8\u28C9\u28CA\u28CB\u28CC\u28CD\u28CE\u28CF"
      "\u28D0\u28D1\u28D2\u28D3\u28D4\u28D5\u28D6\u28D7\u28D8\u28D9\u28DA\u28DB\u28DC\u28DD\u28DE\u28DF"
      "\u28E0\u28E1\u28E2\u28E3\u28E4\u28E5\u28E6\u28E7\u28E8\u28E9\u28EA\u28EB\u28EC\u28ED\u28EE\u28EF"
      "\u28F0\u28F1\u28F2\u28F3\u28F4\u28F5\u28F6\u28F7\u28F8\u28F9\u28FA\u28FB\u28FC\u28FD\u28FE\u28FF"};
  std::cout << "\r\033[" << column << 'C';
  for (int i = 0; i < 6; ++i) {
    index = rng() % (bar.size() / 3);
    std::cout << bar.substr(index * 3, 3);
  }
  (std::cout << '\r').flush();
};

export class Spinner {
  bool has_tty_;
  std::atomic<unsigned> offset_{0};
  std::jthread thread_;
  std::mutex mutex_;
  std::condition_variable_any cv_;
  std::atomic<bool> enabled_{false};
  std::optional<std::tuple<unsigned,
                           std::string_view,
                           std::variant<std::tuple<long, std::string_view>, std::tuple<std::string>>>>
      data_{std::nullopt};

  void Run(std::stop_token const& token) {
    if (not has_tty_) {
      return;
    }
    std::unique_lock lock{mutex_};
    while (not token.stop_requested()) {
      cv_.wait_for(lock, token, std::chrono::milliseconds{50}, [&, this]() { return data_.has_value(); });
      if (data_.has_value()) {
        std::apply(print, *data_);
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

  ~Spinner() {
    thread_.request_stop();
    thread_.join();
  }

  void PutTime(TimeType type, long value, std::string_view units) {
    if (has_tty_) {
      std::unique_lock lock{mutex_};
      cv_.wait(lock, [&] { return not data_.has_value(); });
      data_.emplace(Column(type), Format(type), std::tuple{value, units});
      cv_.notify_one();
    }
  }

  void PutAnswer(AnswerType type, std::string string) {
    if (has_tty_) {
      std::unique_lock lock{mutex_};
      cv_.wait(lock, [&] { return not data_.has_value(); });
      data_.emplace(Column(type), Format(type), std::tuple{std::move(string)});
      cv_.notify_one();
    }
  }

  void Sync() {
    if (has_tty_) {
      std::unique_lock lock{mutex_};
      cv_.wait(lock, [&]() { return not data_.has_value(); });
    }
  }

  void SetLocation(unsigned index) {
    if (has_tty_) {
      offset_.store(index);
    }
  }

  [[nodiscard]] bool HasTTY() const noexcept {
    return has_tty_;
  }

  void Enable() {
    if (has_tty_) {
      enabled_.store(true);
    }
  }

  void Disable() {
    if (has_tty_) {
      enabled_.store(false);
    }
  }
};
