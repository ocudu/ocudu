// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/sink.h"

namespace ocudulog {

/// Standard stream types.
enum class sink_stream_type { stdout, stderr };

/// This sink implementation writes to either stdout or stderr streams.
class stream_sink : public sink
{
public:
  stream_sink(sink_stream_type s, std::unique_ptr<log_formatter> f) :
    sink(std::move(f)), handle((s == sink_stream_type::stdout) ? stdout : stderr)
  {
  }

  stream_sink(const stream_sink& other)            = delete;
  stream_sink& operator=(const stream_sink& other) = delete;

  detail::error_string write(detail::memory_buffer buffer) override
  {
    assert(handle && "Invalid stream handle");
    std::fwrite(buffer.data(), sizeof(char), buffer.size(), handle);
    // We want to see the output instantly.
    std::fflush(handle);
    return {};
  }

  detail::error_string flush() override
  {
    std::fflush(handle);
    return {};
  }

private:
  std::FILE* handle;
};

} // namespace ocudulog
