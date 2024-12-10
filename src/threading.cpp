module;

#include <functional>
#include <ranges>
#include <thread>

#include <cstddef>

#include <pthread.h>

#include <thread_pool/thread_pool.h>

export module threading;

namespace threading {

export unsigned GetNumThreads() {
  static unsigned const THREADS{std::thread::hardware_concurrency() / 2};
  return THREADS;
}

export dp::thread_pool<>& GetThreadPool() {
  static std::optional<dp::thread_pool<>> thread_pool{std::nullopt};
  if (not thread_pool.has_value()) {
    thread_pool.emplace(GetNumThreads(), [](std::size_t core_id) {
      using std::size_t;
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      CPU_SET(core_id, &cpuset);
      pthread_t current_thread = pthread_self();
      pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    });
  }
  return *thread_pool;
}

export template <std::ranges::random_access_range Range>
void ParallelForEach(Range&& range, std::invocable<std::ranges::range_const_reference_t<Range>> auto&& fn, unsigned threads = GetNumThreads()) {
  std::size_t const n{range.size()};
  auto const next_offsets =
      std::views::iota(0u, threads + 1) |
      std::views::transform([&](unsigned i) { return static_cast<long>(i * n) / threads; }) |
      std::ranges::to<std::vector>();
  for (auto const [curr, next] : std::views::adjacent<2>(next_offsets)) {
    GetThreadPool().enqueue_detach([&range, &fn, curr, next] {
      for (auto&& item : std::views::take(std::views::drop(range, curr), next - curr)) {
        (void)std::invoke(fn, std::forward<decltype(item)>(item));
      }
    });
  }
  GetThreadPool().wait_for_tasks();
}

export void Initialize() {
  using std::size_t;
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);
  pthread_t current_thread = pthread_self();
  pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
  ParallelForEach(std::views::iota(0u, GetNumThreads() * 100), [](unsigned const&) {});
}

} // namespace threading
