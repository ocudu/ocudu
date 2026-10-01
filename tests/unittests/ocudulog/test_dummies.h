// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/detail/log_backend.h"
#include "ocudu/ocudulog/shared_types.h"
#include "ocudu/ocudulog/sink.h"

namespace test_dummies {

/// A Dummy implementation of a formatter.
class log_formatter_dummy : public ocudulog::log_formatter
{
public:
  void format(ocudulog::detail::log_entry_metadata&& metadata, fmt::memory_buffer& buffer) override {}

  std::unique_ptr<log_formatter> clone() const override { return nullptr; }

private:
  void format_context_begin(const ocudulog::detail::log_entry_metadata& md,
                            fmt::string_view                            ctx_name,
                            unsigned                                    size,
                            fmt::memory_buffer&                         buffer) override
  {
  }
  void format_context_end(const ocudulog::detail::log_entry_metadata& md,
                          fmt::string_view                            ctx_name,
                          fmt::memory_buffer&                         buffer) override
  {
  }
  void
  format_metric_set_begin(fmt::string_view set_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) override
  {
  }
  void format_metric_set_end(fmt::string_view set_name, unsigned level, fmt::memory_buffer& buffer) override {}
  void format_list_begin(fmt::string_view list_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) override
  {
  }
  void format_list_end(fmt::string_view list_name, unsigned level, fmt::memory_buffer& buffer) override {}
  void format_metric(fmt::string_view      metric_name,
                     fmt::string_view      metric_value,
                     fmt::string_view      metric_units,
                     ocudulog::metric_kind kind,
                     unsigned              level,
                     fmt::memory_buffer&   buffer) override
  {
  }
};

/// A Dummy implementation of a sink.
class sink_dummy : public ocudulog::sink
{
public:
  sink_dummy() : sink(std::unique_ptr<ocudulog::log_formatter>(new log_formatter_dummy)) {}

  ocudulog::detail::error_string write(ocudulog::detail::memory_buffer buffer) override { return {}; }

  ocudulog::detail::error_string flush() override { return {}; }
};

/// A Dummy implementation of the log backend.
class backend_dummy : public ocudulog::detail::log_backend
{
public:
  void start(ocudulog::backend_priority priority) override {}

  bool push(ocudulog::detail::log_entry&& entry) override { return true; }

  bool is_running() const override { return true; }

  fmt::dynamic_format_arg_store<fmt::format_context>* alloc_arg_store() override { return nullptr; }
};

} // namespace test_dummies
