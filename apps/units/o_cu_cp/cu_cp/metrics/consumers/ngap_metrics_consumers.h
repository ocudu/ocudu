// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/mobility_management_metrics.h"
#include "ocudu/ngap/ngap_metrics.h"
#include "ocudu/ocudulog/log_channel.h"

namespace ocudu {

/// Logger consumer for the NGAP metrics.
class ngap_metrics_consumer_log
{
public:
  explicit ngap_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    ocudu_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  /// Handle NGAP metrics.
  void handle_metric(const std::vector<ngap_info>& report, const mobility_management_metrics& mobility_metrics);

private:
  ocudulog::log_channel& log_chan;
};

} // namespace ocudu
