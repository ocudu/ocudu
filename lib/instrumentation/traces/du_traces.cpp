// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/instrumentation/traces/du_traces.h"

ocudu::file_event_tracer<ocudu::L1_DL_TRACE_ENABLED> ocudu::l1_common_tracer;

ocudu::file_event_tracer<ocudu::L1_DL_TRACE_ENABLED> ocudu::l1_dl_tracer;

ocudu::file_event_tracer<ocudu::L1_UL_TRACE_ENABLED> ocudu::l1_ul_tracer;

ocudu::file_event_tracer<ocudu::L2_TRACE_ENABLED> ocudu::l2_tracer;
