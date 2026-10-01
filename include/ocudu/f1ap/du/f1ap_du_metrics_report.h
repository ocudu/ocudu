// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {
namespace odu {

/// Metrics report of the DU F1AP.
struct f1ap_metrics_report {
  struct ue_metrics {
    du_ue_index_t       ue_index;
    gnb_du_ue_f1ap_id_t ue_id;
  };

  /// Number of PDUs received by the F1AP-DU.
  unsigned nof_rx_pdus = 0;
  /// List of UE metrics.
  static_vector<ue_metrics, MAX_NOF_DU_UES> ues;
};

} // namespace odu
} // namespace ocudu
