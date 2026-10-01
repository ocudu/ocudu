// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "fmt/format.h"
#include <unistd.h>
#include <utility>

namespace ocudu {

/// Scoped file descriptor that calls ::close(fd) on destruction (if auto-close is enabled) and is not copyable.
class unique_fd
{
public:
  unique_fd() = default;
  explicit unique_fd(int fd_, bool auto_close_ = true) : fd(fd_), auto_close(auto_close_) {}

  unique_fd(const unique_fd& other)            = delete;
  unique_fd& operator=(const unique_fd& other) = delete;

  unique_fd(unique_fd&& other) noexcept : fd(std::exchange(other.fd, -1)), auto_close(other.auto_close) {}

  unique_fd& operator=(unique_fd&& other) noexcept
  {
    if (is_open() && auto_close && !close()) {
      print_close_error_msg();
    }

    fd         = std::exchange(other.fd, -1);
    auto_close = other.auto_close;
    return *this;
  }

  ~unique_fd()
  {
    if (auto_close && !close()) {
      print_close_error_msg();
    }
  }

  /// Closes the file descriptor if it is open. Returns true on success, otherwise false.
  [[nodiscard]] bool close()
  {
    if (fd >= 0) {
      if (::close(fd) == -1) {
        fd = -1;
        return false;
      }
      fd = -1;
    }
    return true;
  }

  /// Returns true if the file descriptor is open.
  [[nodiscard]] bool is_open() const { return fd >= 0; }

  /// Returns the raw file descriptor value or -1 if the file descriptor is not open.
  [[nodiscard]] int value() const { return fd; }

  /// Releases the ownership of the managed file descriptor.
  [[nodiscard]] int release() { return std::exchange(fd, -1); }

private:
  /// Prints a close error message with the corresponding cause.
  void print_close_error_msg()
  {
    fmt::print("Error: Failed to close file descriptor '{}': {}.\n", fd, ::strerror(errno));
  }

  int  fd         = -1;
  bool auto_close = true;
};

} // namespace ocudu
