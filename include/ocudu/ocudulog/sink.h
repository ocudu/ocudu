// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/detail/support/error_string.h"
#include "ocudu/ocudulog/detail/support/memory_buffer.h"
#include "ocudu/ocudulog/formatter.h"
#include <cassert>

namespace ocudulog {

/// This interface provides the way to write incoming memory buffers to any kind
/// of backing store.
class sink
{
public:
  explicit sink(std::unique_ptr<log_formatter> f) : formatter(std::move(f))
  {
    assert(formatter && "Invalid formatter");
  }

  virtual ~sink() = default;

  /// Returns the formatter used by this sink.
  log_formatter&       get_formatter() { return *formatter; }
  const log_formatter& get_formatter() const { return *formatter; }

  /// Writes the provided memory buffer into the sink.
  virtual detail::error_string write(detail::memory_buffer buffer) = 0;

  /// Writes as an error the provided memory buffer into the sink.
  virtual detail::error_string write_error(detail::memory_buffer buffer) { return {}; }

  /// Flushes any buffered contents to the backing store.
  virtual detail::error_string flush() = 0;

private:
  std::unique_ptr<log_formatter> formatter;
};

} // namespace ocudulog
