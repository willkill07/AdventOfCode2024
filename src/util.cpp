#include "util.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace util {

FileDescriptor::FileDescriptor(char const* filename) : fd_{open(filename, O_RDONLY)} {
  if (fd_ < 0) {
    throw std::invalid_argument{"invalid file"};
  }
}

FileDescriptor::~FileDescriptor() noexcept {
  if (not(fd_ < 0)) {
    close(fd_);
  }
}

Buffer::Buffer(const char* filename)
    : fd_{filename}, span{[this] {
        if (struct stat file_stat; fstat(fd_, &file_stat) < 0) {
          throw std::invalid_argument{"invalid file"};
        } else {
          size_t const bytes{static_cast<size_t>(file_stat.st_size)};
          if (char const* address =
                  reinterpret_cast<char const*>(mmap(NULL, bytes, PROT_READ, MAP_PRIVATE | MAP_FILE, fd_, 0));
              address == nullptr) {
            throw std::invalid_argument{"invalid file"};
          } else {
            return std::span{address, bytes};
          }
        }
      }()} {
}

Buffer::~Buffer() noexcept {
  if (char const* addr = span.data(); addr != nullptr) {
    (void)munmap((void*)addr, span.size());
  }
}

} // namespace util
