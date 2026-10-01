// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <string>

namespace ocudulog {

namespace detail {

/// This class wraps a read-only and non owning memory block, providing simple
/// methods to access its contents.
class memory_buffer
{
  const char* const buffer;
  const size_t      length;

public:
  memory_buffer(const char* buffer_, size_t length_) : buffer(buffer_), length(length_) {}

  explicit memory_buffer(const std::string& s) : buffer(s.data()), length(s.size()) {}

  /// Returns a pointer to the start of the memory block.
  const char* data() const { return buffer; }

  /// Returns an iterator to the beginning of the buffer.
  const char* begin() const { return buffer; }

  /// Returns an iterator to the end of the buffer.
  const char* end() const { return buffer + length; }

  /// Returns the size of the memory block.
  size_t size() const { return length; }
};

} // namespace detail

} // namespace ocudulog
