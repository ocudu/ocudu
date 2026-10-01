// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocudulog/ocudulog_c.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstdarg>

using namespace ocudulog;

template <typename To, typename From>
static inline To* c_cast(From* x)
{
  return reinterpret_cast<To*>(x);
}

/// Helper to format the input argument list writing it into a channel.
static void log_to(log_channel& c, const char* fmt, std::va_list args)
{
  char buffer[1024];
  std::vsnprintf(buffer, sizeof(buffer) - 1, fmt, args);
  c(buffer);
}

void ocudulog_init(void)
{
  init();
}

void ocudulog_set_default_sink(ocudulog_sink* s)
{
  assert(s && "Expected a valid sink");
  set_default_sink(*c_cast<sink>(s));
}

ocudulog_sink* ocudulog_get_default_sink(void)
{
  return c_cast<ocudulog_sink>(&get_default_sink());
}

ocudulog_log_channel* ocudulog_fetch_log_channel(const char* id)
{
  return c_cast<ocudulog_log_channel>(&fetch_log_channel(id));
}

ocudulog_log_channel* ocudulog_find_log_channel(const char* id)
{
  return c_cast<ocudulog_log_channel>(find_log_channel(id));
}

void ocudulog_set_log_channel_enabled(ocudulog_log_channel* channel, ocudulog_bool enabled)
{
  assert(channel && "Expected a valid channel");
  c_cast<log_channel>(channel)->set_enabled(enabled);
}

ocudulog_bool ocudulog_is_log_channel_enabled(ocudulog_log_channel* channel)
{
  assert(channel && "Expected a valid channel");
  return c_cast<log_channel>(channel)->enabled();
}

const char* ocudulog_get_log_channel_id(ocudulog_log_channel* channel)
{
  assert(channel && "Expected a valid channel");
  return c_cast<log_channel>(channel)->id().c_str();
}

void ocudulog_log(ocudulog_log_channel* channel, const char* fmt, ...)
{
  assert(channel && "Expected a valid channel");

  std::va_list args;
  va_start(args, fmt);
  log_to(*c_cast<log_channel>(channel), fmt, args);
  va_end(args);
}

ocudulog_logger* ocudulog_fetch_default_logger(const char* id)
{
  return c_cast<ocudulog_logger>(&fetch_basic_logger(id));
}

ocudulog_logger* ocudulog_find_default_logger(const char* id)
{
  return c_cast<ocudulog_logger>(find_logger<basic_logger>(id));
}

void ocudulog_debug(ocudulog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->debug, fmt, args);
  va_end(args);
}

void ocudulog_info(ocudulog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->info, fmt, args);
  va_end(args);
}

void ocudulog_warning(ocudulog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->warning, fmt, args);
  va_end(args);
}

void ocudulog_error(ocudulog_logger* log, const char* fmt, ...)
{
  assert(log && "Expected a valid logger");

  std::va_list args;
  va_start(args, fmt);
  log_to(c_cast<basic_logger>(log)->error, fmt, args);
  va_end(args);
}

const char* ocudulog_get_logger_id(ocudulog_logger* log)
{
  assert(log && "Expected a valid logger");
  return c_cast<basic_logger>(log)->id().c_str();
}

/// Translate the C API level enum to basic_levels.
static basic_levels convert_c_enum_to_basic_levels(ocudulog_log_levels lvl)
{
  switch (lvl) {
    case ocudulog_lvl_none:
      return basic_levels::none;
    case ocudulog_lvl_debug:
      return basic_levels::debug;
    case ocudulog_lvl_info:
      return basic_levels::info;
    case ocudulog_lvl_warning:
      return basic_levels ::warning;
    case ocudulog_lvl_error:
      return basic_levels::error;
  }

  assert(false && "Invalid enum value");
  return basic_levels::none;
}

void ocudulog_set_logger_level(ocudulog_logger* log, ocudulog_log_levels lvl)
{
  assert(log && "Expected a valid logger");
  c_cast<basic_logger>(log)->set_level(convert_c_enum_to_basic_levels(lvl));
}

ocudulog_sink* ocudulog_find_sink(const char* id)
{
  return c_cast<ocudulog_sink>(find_sink(id));
}

ocudulog_sink* ocudulog_fetch_stdout_sink(void)
{
  return c_cast<ocudulog_sink>(&fetch_stdout_sink());
}

ocudulog_sink* ocudulog_fetch_stderr_sink(void)
{
  return c_cast<ocudulog_sink>(&fetch_stderr_sink());
}

ocudulog_sink* ocudulog_fetch_file_sink(const char* path, size_t max_size, ocudulog_bool force_flush)
{
  return c_cast<ocudulog_sink>(&fetch_file_sink(path, max_size, force_flush));
}
