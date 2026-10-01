// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/consumers/ngap_metrics_consumers.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/consumers/rrc_metrics_consumers.h"
#include "ocudu/ocudulog/log_channel.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

namespace app_services {
class remote_server_metrics_gateway;
} // namespace app_services

class e2_cu_metrics_notifier;

/// Consumer for the json CU-CP metrics.
class cu_cp_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_json(app_services::remote_server_metrics_gateway& gateway_) : gateway(gateway_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  app_services::remote_server_metrics_gateway& gateway;
};

/// Consumer for the log CU-CP metrics.
class cu_cp_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_log(ocudulog::log_channel& log_chan_) :
    ngap_consumer(log_chan_), rrc_consumer(log_chan_)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ngap_metrics_consumer_log ngap_consumer;
  rrc_metrics_consumer_log  rrc_consumer;
};

/// E2 Consumer for the CU-CP metrics.
class cu_cp_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_e2(e2_cu_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  e2_cu_metrics_notifier& notifier;
};

} // namespace ocudu
