// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/tracing/event_tracing.h"
#include <vector>

namespace ocudu {

/// \brief Helper class to register all the sections within a recording period.
///
/// The events are only logged if the time difference between stop_recording and start_recording is higher than
/// \c latency_thres.
/// \remark This class is not thread safe.
template <typename TracerType, bool Enabled = not std::is_base_of_v<detail::null_event_tracer, TracerType>>
class rusage_trace_recorder
{
public:
  template <typename T = TracerType>
  rusage_trace_recorder(const std::string& name_, T&& tracer_, trace_duration latency_thres_, unsigned max_sections) :
    name(name_), tracer(std::forward<T>(tracer_)), thres(latency_thres_)
  {
    if (tracer.is_enabled()) {
      // Add two extra positions for start and stop.
      trace_points.reserve(max_sections + 2);
    }
  }

  void start()
  {
    if (tracer.is_enabled()) {
      ocudu_assert(trace_points.empty(), "Recorder has already started");
      trace_points.emplace_back(rusage_trace_event{name.c_str(), this->tracer.now(), this->tracer.rusage_now()});
    }
  }

  void add_section(const char* section_name)
  {
    if (not tracer.is_enabled() or trace_points.size() + 1 >= trace_points.capacity()) {
      // No more space (one entry needs to be left for the stop).
      return;
    }
    ocudu_assert(not trace_points.empty(), "Recorder did not start");
    trace_points.emplace_back(rusage_trace_event{section_name, this->tracer.now(), this->tracer.rusage_now()});
  }

  void stop(const char* stop_section_name)
  {
    if (not tracer.is_enabled()) {
      return;
    }
    ocudu_assert(not trace_points.empty(), "Recorder did not start");
    auto end_tp = this->tracer.now();
    auto dur    = std::chrono::duration_cast<trace_duration>(end_tp - this->trace_points.front().start_tp);
    if (dur >= this->thres) {
      // We exceeded the max latency.
      trace_points.emplace_back(rusage_trace_event{stop_section_name, end_tp, this->tracer.rusage_now()});
      tracer << trace_points;
    }
    trace_points.clear();
  }

private:
  std::string    name;
  TracerType     tracer;
  trace_duration thres;

  std::vector<rusage_trace_event> trace_points;
};

// Specialization for when the tracer is disabled.
template <typename TracerType>
class rusage_trace_recorder<TracerType, false>
{
public:
  template <typename... TracerArgs>
  rusage_trace_recorder(TracerArgs&&... /* unused */)
  {
  }

  void start() {}
  void add_section(const char* /* unused */) {}
  void stop(const char* /* unused */) {}
};

template <typename TracerType>
rusage_trace_recorder<TracerType> create_rusage_trace_recorder(const std::string& name,
                                                               TracerType&&       tracer,
                                                               trace_duration     latency_thres,
                                                               unsigned           max_sections)
{
  return rusage_trace_recorder<TracerType>{name, std::forward<TracerType>(tracer), latency_thres, max_sections};
}

} // namespace ocudu
