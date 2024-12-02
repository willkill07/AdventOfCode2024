#pragma once

#include <span>

namespace util {

struct FileDescriptor {
  FileDescriptor(char const* filename);
  ~FileDescriptor() noexcept;

  FileDescriptor() = delete;
  FileDescriptor(FileDescriptor const&) = delete;
  FileDescriptor(FileDescriptor&&) = delete;
  FileDescriptor& operator=(FileDescriptor const&) = delete;
  FileDescriptor& operator=(FileDescriptor&&) = delete;

  [[nodiscard]] inline operator int() const noexcept {
    return fd_;
  }

private:
  int const fd_;
};

struct Buffer {
  Buffer(const char* filename);
  ~Buffer() noexcept;

  Buffer(Buffer const&) = delete;
  Buffer(Buffer&&) = delete;
  Buffer& operator=(Buffer const&) = delete;
  Buffer& operator=(Buffer&&) = delete;

private:
  FileDescriptor const fd_;

public:
  std::span<char const> const span;
};

} // namespace util
