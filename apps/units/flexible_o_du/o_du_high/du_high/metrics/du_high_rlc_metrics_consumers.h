// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_high_rlc_metrics.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

/// Consumer for the json RLC metrics.
class rlc_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) : gateway(gateway_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  app_services::remote_server_metrics_gateway& gateway;
};

/// Consumer for the log RLC metrics.
class rlc_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_log(ocudulog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    ocudu_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ocudulog::log_channel& log_chan;
};

/// Consumer for the E2 RLC metrics.
class rlc_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_e2(rlc_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  rlc_metrics_notifier& notifier;
};

} // namespace ocudu
