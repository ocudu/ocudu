// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocudulog/event_trace.h"
#include "sinks/buffered_file_sink.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <ctime>

#undef trace_duration_begin
#undef trace_duration_end
#undef trace_complete_event

using namespace ocudulog;

/// Log channel where event traces will get sent.
static log_channel* tracer = nullptr;

/// Tracer sink name.
static constexpr char sink_name[] = "ocudulog_trace_sink";

void ocudulog::event_trace_init()
{
  // Nothing to do if the user previously set a custom channel or this is not
  // the first time this function is called.
  if (tracer) {
    return;
  }

  // Default file name where event traces will get stored.
  static constexpr char default_file_name[] = "event_trace.log";

  // Create the default event trace channel.
  //: TODO: handle name reservation.
  sink* s = create_file_sink(default_file_name);
  assert(s && "Default event file sink is reserved");
  tracer = create_log_channel("event_trace_channel", *s);
  assert(tracer && "Default event trace channel is reserved");
}

void ocudulog::event_trace_init(log_channel& c)
{
  // Nothing to set when a channel has already been installed.
  if (!tracer) {
    tracer = &c;
  }
}

bool ocudulog::event_trace_init(const std::string& filename, std::size_t capacity)
{
  // Nothing to do if the user previously set a custom channel or this is not
  // the first time this function is called.
  if (tracer) {
    return false;
  }

  auto tracer_sink = std::unique_ptr<sink>(new buffered_file_sink(filename, capacity, get_default_log_formatter()));
  if (!install_custom_sink(sink_name, std::move(tracer_sink))) {
    return false;
  }

  if (sink* s = find_sink(sink_name)) {
    log_channel& c = fetch_log_channel("event_trace_channel", *s, {"TRACE", '\0', false});
    tracer         = &c;
    return true;
  }

  return false;
}

/// Fills in the input buffer with the current time.
static void format_time(char* buffer, size_t len)
{
  std::time_t t = std::time(nullptr);
  std::tm     lt{};
  ::localtime_r(&t, &lt);
  std::strftime(buffer, len, "%FT%T", &lt);
}

namespace ocudulog {

void trace_duration_begin(const std::string& category, const std::string& name)
{
  if (!tracer) {
    return;
  }

  char fmt_time[24];
  format_time(fmt_time, sizeof(fmt_time));
  (*tracer)("[{}] [TID:{}] Entering \"{}\": {}", fmt_time, (unsigned)::pthread_self(), category, name);
}

void trace_duration_end(const std::string& category, const std::string& name)
{
  if (!tracer) {
    return;
  }

  char fmt_time[24];
  format_time(fmt_time, sizeof(fmt_time));
  (*tracer)("[{}] [TID:{}] Leaving \"{}\": {}", fmt_time, (unsigned)::pthread_self(), category, name);
}

} // namespace ocudulog

/// Private implementation of the complete event destructor.
ocudulog::detail::scoped_complete_event::~scoped_complete_event()
{
  if (!tracer) {
    return;
  }

  auto end  = std::chrono::steady_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  if (diff < threshold) {
    return;
  }

  (*tracer)("{} {}, {}", category, name, (unsigned)diff.count());
}
