// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u_metrics_aggregator.h"
#include "ocudu/f1u/cu_up/f1u_bearer.h"

using namespace ocudu::ocuup;

f1u_metrics_aggregator::f1u_metrics_aggregator(uint32_t                       ue_index_,
                                               drb_id_t                       drb_id_,
                                               const up_transport_layer_info& ul_tnl_info_,
                                               timer_duration                 metrics_period_,
                                               f1u_metrics_notifier*          f1u_metrics_notif_,
                                               task_executor&                 ue_ctrl_executor_,
                                               bool                           report_tx_rx_in_same_report_) :
  report_tx_rx_in_same_report(report_tx_rx_in_same_report_),
  ue_index(ue_index_),
  drb_id(drb_id_),
  metrics_period(metrics_period_),
  f1u_metrics_notif(f1u_metrics_notif_),
  ue_ctrl_executor(ue_ctrl_executor_),
  logger("CU-F1-U", {ue_index_, drb_id_, ul_tnl_info_})
{
  // Initialize to different values so the first call to push metrics does not force a metrics report until tx and rx
  // are present.
  m_tx.counter = 10;
  m_rx.counter = 50;
}

void f1u_metrics_aggregator::push_tx_metrics(const f1u_tx_metrics_container& m_tx_)
{
  if (not ue_ctrl_executor.execute([this, m_tx_]() { push_tx_metrics_impl(m_tx_); })) {
    logger.log_error("Could not push TX metrics");
  }
}

void f1u_metrics_aggregator::push_rx_metrics(const f1u_rx_metrics_container& m_rx_)
{
  if (not ue_ctrl_executor.execute([this, m_rx_]() { push_rx_metrics_impl(m_rx_); })) {
    logger.log_error("Could not push RX metrics");
  }
}

void f1u_metrics_aggregator::push_tx_metrics_impl(const f1u_tx_metrics_container& m_tx_)
{
  m_tx = m_tx_;
  push_report();
}

void f1u_metrics_aggregator::push_rx_metrics_impl(const f1u_rx_metrics_container& m_rx_)
{
  m_rx = m_rx_;
  push_report();
}

void f1u_metrics_aggregator::push_report()
{
  // Notify tx and rx in the same F1-U metrics container.
  if (report_tx_rx_in_same_report && m_rx.counter != m_tx.counter) {
    return;
  }

  f1u_metrics_container metrics = {ue_index, drb_id, m_tx, m_rx, metrics_period};
  f1u_metrics_notif->report_metrics(metrics);
}
