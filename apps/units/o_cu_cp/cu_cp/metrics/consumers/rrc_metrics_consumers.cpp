// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_metrics_consumers.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/format/fmt_to_c_str.h"

using namespace ocudu;

void rrc_metrics_consumer_log::handle_metric(const std::vector<cu_cp_metrics_report::du_info>& report,
                                             const mobility_management_metrics&                mobility_metrics)
{
  if (report.empty()) {
    return;
  }

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "CU-CP RRC metrics: {}", format_rrc_metrics(report, mobility_metrics));

  log_chan("{}", to_c_str(buffer));
  buffer.clear();
}
