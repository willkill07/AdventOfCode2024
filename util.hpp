#pragma once


#include <span>

namespace util {

struct file_descriptor {
  file_descriptor(char const *filename);
  ~file_descriptor() noexcept;

  file_descriptor() = delete;
  file_descriptor(file_descriptor const &) = delete;
  file_descriptor(file_descriptor &&) = delete;
  file_descriptor &operator=(file_descriptor const &) = delete;
  file_descriptor &operator=(file_descriptor &&) = delete;

  [[nodiscard]] inline operator int() const noexcept { return fd_; }

private:
  int const fd_;
};

struct buffer {
  buffer(const char *filename);
  ~buffer() noexcept;

  buffer(buffer const &) = delete;
  buffer(buffer &&) = delete;
  buffer &operator=(buffer const &) = delete;
  buffer &operator=(buffer &&) = delete;

private:
  file_descriptor const fd_;

public:
  std::span<char const> const span;
};

} // namespace util
