#pragma once

#include <functional>
#include <ranges>
#include <thread>

#include <thread_pool/thread_pool.h>

static inline unsigned const threads{std::thread::hardware_concurrency()};

static inline dp::thread_pool<>& GetThreadPool() {
  static dp::thread_pool<> thread_pool{threads};
  return thread_pool;
}

template <std::ranges::random_access_range Range>
inline void ParallelForEach(Range&& range,
                     std::move_only_function<void(std::ranges::range_const_reference_t<Range>)> fn) {
  std::size_t const n{range.size()};
  long curr{0};
  for (unsigned i : std::views::iota(0u, threads)) {
    long const next{static_cast<long>((i + 1) * n / threads)};
    GetThreadPool().enqueue_detach([&fn, &range, curr, next]() {
      for (auto& item : std::views::take(std::views::drop(range, curr), next - curr)) {
        std::invoke(fn, item);
      }
    });
    curr = next;
  }
  GetThreadPool().wait_for_tasks();
}
