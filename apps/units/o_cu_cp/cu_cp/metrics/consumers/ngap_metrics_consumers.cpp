// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_metrics_consumers.h"

using namespace ocudu;

void ngap_metrics_consumer_log::handle_metric(const std::vector<ngap_info>&      report,
                                              const mobility_management_metrics& mobility_metrics)
{
  if (report.empty()) {
    return;
  }

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "CU-CP NGAP metrics: {}", format_ngap_metrics(report, mobility_metrics));

  log_chan("{}", to_c_str(buffer));
  buffer.clear();
}
