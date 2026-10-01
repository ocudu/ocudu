// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_metrics_collector_impl.h"

using namespace ocudu;
using namespace odu;

f1ap_metrics_collector_impl::f1ap_metrics_collector_impl(bool enabled) : enabled_flag(enabled) {}

void f1ap_metrics_collector_impl::collect_metrics_report(f1ap_metrics_report& report)
{
  report = {};
  if (not enabled_flag) {
    return;
  }

  // Prepare report.
  report.nof_rx_pdus = data.nof_rx_pdus;
  for (const auto& u : ues) {
    report.ues.push_back(u);
  }

  // Reset report data.
  data = {};
}

void f1ap_metrics_collector_impl::on_ue_creation(du_ue_index_t ue_index, gnb_du_ue_f1ap_id_t du_ue_id)
{
  if (not enabled_flag) {
    return;
  }

  ues.emplace(ue_index, f1ap_metrics_report::ue_metrics{ue_index, du_ue_id});
}

void f1ap_metrics_collector_impl::on_ue_removal(du_ue_index_t ue_index)
{
  if (not enabled_flag) {
    return;
  }

  ues.erase(ue_index);
}

void f1ap_metrics_collector_impl::on_rx_pdu(const f1ap_message& msg)
{
  if (not enabled_flag) {
    return;
  }

  data.nof_rx_pdus++;
}
