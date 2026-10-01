// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_procedure_metrics_consumers.h"
#include "ocudu/support/format/fmt_to_c_str.h"

using namespace ocudu;

void du_procedure_metrics_consumer_log::handle_metric(const std::optional<odu::du_procedure_metrics_report>& report)
{
  if (!report) {
    return;
  }

  fmt::memory_buffer buffer;

  fmt::format_to(std::back_inserter(buffer),
                 "DU manager metrics: nof_ue_creations={} avg_ue_creation_latency={}usec "
                 "max_ue_creation_latency={}usec",
                 report->nof_ue_creations,
                 std::chrono::duration_cast<std::chrono::microseconds>(report->average_ue_creation_latency).count(),
                 std::chrono::duration_cast<std::chrono::microseconds>(report->max_ue_creation_latency).count());

  log_chan("{}", to_c_str(buffer));
}
