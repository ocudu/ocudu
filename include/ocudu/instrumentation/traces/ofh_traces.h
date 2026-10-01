// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/support/tracing/event_tracing.h"

namespace ocudu {

/// Set to true for enabling OFH trace.
#ifndef OCUDU_OFH_TRACE
constexpr bool OFH_TRACE_ENABLED = false;
#else
constexpr bool OFH_TRACE_ENABLED = true;
#endif

/// OFH event tracing. This tracer is used to analyze latencies in the OFH processing.
extern file_event_tracer<OFH_TRACE_ENABLED> ofh_tracer;

} // namespace ocudu
