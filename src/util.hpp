#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace util {

template <typename Fn> struct OnScopeExit {

  explicit OnScopeExit(Fn&& f) : fn_{std::move(f)} {
  }

  OnScopeExit(OnScopeExit const&) = delete;
  OnScopeExit(OnScopeExit&&) = delete;
  OnScopeExit& operator=(OnScopeExit const&) = delete;
  OnScopeExit& operator=(OnScopeExit&&) = delete;
  ~OnScopeExit() {
    std::invoke(fn_);
  }

private:
  Fn fn_;
};

[[nodiscard]] std::string ReadFile(const char* filename);

} // namespace util
