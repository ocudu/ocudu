// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/support/tracing/rusage_trace_recorder.h"

namespace ocudu {

/// General event tracing for critical events such as real-time errors.
extern file_event_tracer<true> general_critical_tracer;

} // namespace ocudu
