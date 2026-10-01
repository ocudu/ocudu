// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/tracing/event_tracing.h"

namespace ocudu {

/// Set to true for enabling radio unit trace.
#ifndef OCUDU_RU_TRACE
constexpr bool RU_TRACE_ENABLED = false;
#else
constexpr bool RU_TRACE_ENABLED = true;
#endif

/// RU event tracing. This tracer is used to analyze latencies in the RU processing.
extern file_event_tracer<RU_TRACE_ENABLED> ru_tracer;

} // namespace ocudu
