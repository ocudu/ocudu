// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/tracing/event_tracing.h"

namespace ocudu {

/// Set to true for enabling user-plane related traces.
#ifndef OCUDU_UP_TRACE
constexpr bool UP_TRACE_ENABLED = false;
#else
constexpr bool UP_TRACE_ENABLED = true;
#endif

/// UP event tracing. This tracer is used to analyze latencies in the CU-UP/DU high processing
/// of the user plane traffic.
extern file_event_tracer<UP_TRACE_ENABLED> up_tracer;

} // namespace ocudu
