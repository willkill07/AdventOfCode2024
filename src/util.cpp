#include "util.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace util {

std::string ReadFile(const char* filename) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    throw std::invalid_argument{"invalid file"};
  }
  OnScopeExit _{[&] { (void)close(fd); }};
  if (struct stat file_stat; fstat(fd, &file_stat) < 0) {
    throw std::invalid_argument{"invalid file"};
  } else {
    size_t const bytes{static_cast<size_t>(file_stat.st_size)};
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
