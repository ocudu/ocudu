// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <string>

namespace ocudu {

/// Configuration of the tracing service.
struct tracer_appconfig {
  /// Set to a valid file path to enable tracing and write the trace to the file.
  std::string filename;
  /// Maximum number of events per file. Set to zero for no limit.
  unsigned max_tracing_events_per_file = 1e6;
  /// Number of events to write prior to a severe event. Set to zero for writing all events.
  unsigned nof_tracing_events_before_severe = 0;
};

} // namespace ocudu
