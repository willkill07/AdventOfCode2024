module;

#include <functional>
#include <ranges>
#include <thread>

#include <thread_pool/thread_pool.h>

export module threading;

namespace threading {

static unsigned const threads{std::thread::hardware_concurrency()};

export dp::thread_pool<>& GetThreadPool() {
  static dp::thread_pool<> thread_pool{threads};
  return thread_pool;
}

export template <std::ranges::random_access_range Range>
void ParallelForEach(Range&& range, std::invocable<std::ranges::range_const_reference_t<Range>> auto&& fn) {
  std::size_t const n{range.size()};
  long curr{0};
  for (unsigned i : std::views::iota(0u, threads)) {
    long const next{static_cast<long>((i + 1) * n / threads)};
    GetThreadPool().enqueue_detach(
        [&](long curr, long next) {
          for (auto& item : std::views::take(std::views::drop(range, curr), next - curr)) {
            (void)std::invoke(fn, item);
          }
        },
        curr,
        next);
    curr = next;
  }
  GetThreadPool().wait_for_tasks();
}

} // namespace threading
