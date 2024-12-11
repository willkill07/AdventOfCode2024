module;

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

export module spinner;

using std::string_view_literals::operator""sv;
constexpr static std::array bar{"⣾"sv, "⣷"sv, "⣯"sv, "⣟"sv, "⡿"sv, "⢿"sv, "⣻"sv, "⣽"sv};

export class Spinner {
  bool has_tty_;
  std::atomic<unsigned> offset_{0};
  std::jthread thread_;
  std::atomic<bool> enabled_{false};

public:
  Spinner(bool has_tty)
      : has_tty_{has_tty}, thread_{[this](std::stop_token token) {
          if (not has_tty_) {
            return;
          }
          unsigned i{0};
          std::mutex mutex;
          std::unique_lock lock{mutex};
          std::condition_variable_any cv;
          while (not token.stop_requested()) {
            cv.wait_for(lock, token, std::chrono::milliseconds{50}, [] { return false; });
            if (enabled_.load()) {
              i = (i + 1) & 7;
              unsigned const col{offset_.load()};
              (std::cout << "\r\033[" << col << 'C' << bar[i] << '\r').flush();
            }
          }
        }} {
  }

  ~Spinner() {
    thread_.request_stop();
    thread_.join();
  }

  void SetLocation(unsigned index) {
    if (has_tty_) {
      offset_.store(index);
    }
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
