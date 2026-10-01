// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_bearer_logger.h"
#include "ocudu/f1u/cu_up/f1u_metrics.h"
#include "ocudu/f1u/cu_up/f1u_rx_metrics.h"
#include "ocudu/f1u/cu_up/f1u_tx_metrics.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"

namespace ocudu::ocuup {
class f1u_metrics_aggregator
{
public:
  f1u_metrics_aggregator(uint32_t                       ue_index_,
                         drb_id_t                       drb_id_,
                         const up_transport_layer_info& ul_tnl_info_,
                         timer_duration                 metrics_period_,
                         f1u_metrics_notifier*          f1u_metrics_notif_,
                         task_executor&                 ue_ctrl_executor_,
                         bool                           report_tx_rx_in_same_report_ = true);

  void push_tx_metrics(const f1u_tx_metrics_container& m_tx_);

  void push_rx_metrics(const f1u_rx_metrics_container& m_rx_);

  const timer_duration& get_metrics_period() const { return metrics_period; }

private:
  void push_report();

  void push_tx_metrics_impl(const f1u_tx_metrics_container& m_tx_);
  void push_rx_metrics_impl(const f1u_rx_metrics_container& m_rx_);

  const bool               report_tx_rx_in_same_report;
  uint32_t                 ue_index;
  drb_id_t                 drb_id;
  f1u_rx_metrics_container m_rx;
  f1u_tx_metrics_container m_tx;
  timer_duration           metrics_period;
  f1u_metrics_notifier*    f1u_metrics_notif;
  task_executor&           ue_ctrl_executor;
  f1u_bearer_logger        logger;
};
} // namespace ocudu::ocuup
