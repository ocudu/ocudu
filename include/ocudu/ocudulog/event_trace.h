// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <chrono>
#include <string>

namespace ocudulog {

class log_channel;

/// The event tracing framework allows to trace any kind of event inside an
/// application.
/// To enable event tracing the ENABLE_OCUDULOG_EVENT_TRACE macro symbol should
/// be defined, otherwise calls to the tracing framework will be ignored. This
/// is important to avoid the overhead of tracing when it is not required.
/// For details about each event trace type please refer to:
/// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit

/// Initializes the event trace framework saving the trace events to a
/// "event_trace.log" file.
void event_trace_init();

/// Initializes the event trace framework using the specified log channel to log
/// all trace events.
void event_trace_init(log_channel& c);

/// Initializes the event trace framework.
/// The event trace data will be written into the specified filename after
/// capacity bytes of data have been generated or at program exit.
/// Returns true on success, otherwise false.
bool event_trace_init(const std::string& filename, std::size_t capacity = 1024 * 1024);

#ifdef ENABLE_OCUDULOG_EVENT_TRACE

/// Generates the begin phase of a duration event.
void trace_duration_begin(const std::string& category, const std::string& name);

/// Generates the end phase of a duration event.
void trace_duration_end(const std::string& category, const std::string& name);

#define OCUDULOG_TRACE_COMBINE1(X, Y) X##Y
#define OCUDULOG_TRACE_COMBINE(X, Y) OCUDULOG_TRACE_COMBINE1(X, Y)

/// Generates a complete event.
#define trace_complete_event(C, N)                                                                                     \
  auto OCUDULOG_TRACE_COMBINE(scoped_complete_event, __LINE__) = ocudulog::detail::scoped_complete_event(C, N)

/// Generates a complete event but only is recorded if the event duration exceeds a certain threshold in microseconds.
#define trace_threshold_complete_event(C, N, T)                                                                        \
  auto OCUDULOG_TRACE_COMBINE(scoped_complete_event, __LINE__) = ocudulog::detail::scoped_complete_event(C, N, T)

#else

/// No-ops.
#define trace_duration_begin(C, N)
#define trace_duration_end(C, N)
#define trace_complete_event(C, N)
#define trace_threshold_complete_event(C, N, T)

#endif

namespace detail {

/// Scoped type object for implementing a complete event.
class scoped_complete_event
{
public:
  scoped_complete_event(const char*               cat,
                        const char*               n,
                        std::chrono::microseconds threshold_ = std::chrono::microseconds::zero()) :
    category(cat), name(n), start(std::chrono::steady_clock::now()), threshold(threshold_)
  {
  }

  ~scoped_complete_event();

private:
  const char* const                                  category;
  const char* const                                  name;
  std::chrono::time_point<std::chrono::steady_clock> start;
  std::chrono::microseconds                          threshold;
};

} // namespace detail

} // namespace ocudulog
