// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_metrics.h"
#include "ocudu/ocudulog/log_channel.h"

namespace ocudu {

/// Logger consumer for the MAC metrics.
class mac_metrics_consumer_log
{
public:
  explicit mac_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_) {}

  /// Handle the given DU high MAC metrics.
  void handle_metric(const mac_dl_metric_report& report);

private:
  ocudulog::log_channel& log_chan;
};

} // namespace ocudu
