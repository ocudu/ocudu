// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/f1ap/du/f1ap_du_metrics_collector.h"

namespace ocudu {

struct f1ap_message;

namespace odu {

/// Collector of F1AP metrics.
class f1ap_metrics_collector_impl final : public f1ap_metrics_collector
{
public:
  f1ap_metrics_collector_impl(bool enabled);

  void collect_metrics_report(f1ap_metrics_report& report) override;

  /// Called on every UE creation in the F1AP.
  void on_ue_creation(du_ue_index_t ue_index, gnb_du_ue_f1ap_id_t du_ue_id);

  /// Called on every UE removal in the F1AP.
  void on_ue_removal(du_ue_index_t ue_index);

  /// Called on every received PDU from the F1-C.
  void on_rx_pdu(const f1ap_message& msg);

private:
  struct non_persistent_common_data {
    unsigned nof_rx_pdus = 0;
  };

  bool enabled_flag;

  non_persistent_common_data                                        data;
  slotted_id_vector<du_ue_index_t, f1ap_metrics_report::ue_metrics> ues;
};

} // namespace odu
} // namespace ocudu
