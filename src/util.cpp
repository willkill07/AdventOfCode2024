
module;

#include <functional>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

export module util;

export namespace util {

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

[[nodiscard]] std::string ReadFile(const char* filename) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    throw std::invalid_argument{"invalid file"};
  }
  OnScopeExit _{[&] { (void)close(fd); }};
  if (struct stat file_stat; fstat(fd, &file_stat) < 0) {
    throw std::invalid_argument{"invalid file"};
  } else {
    std::size_t const bytes{static_cast<std::size_t>(file_stat.st_size)};
    if (char const* address =
            reinterpret_cast<char const*>(mmap(nullptr, bytes, PROT_READ, MAP_PRIVATE | MAP_FILE, fd, 0));
        address == nullptr) {
      throw std::invalid_argument{"invalid file"};
    } else {
      std::string s{address, bytes};
      (void)munmap((void*)address, bytes);
      return s;
    }
  }
}

} // namespace util
