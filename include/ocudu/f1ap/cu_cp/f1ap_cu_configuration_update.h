// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/common/interface_management.h"
#include "ocudu/ran/cause/f1ap_cause.h"
#include <vector>

namespace ocudu {
namespace ocucp {

/// Common type F1AP GNB-CU Configuration Update, see TS 38.473, section 9.2.1.10.
struct f1ap_gnb_cu_configuration_update {
  std::vector<f1ap_cell_to_activate>   cells_to_be_activated_list;
  std::vector<f1ap_cell_to_deactivate> cells_to_be_deactivated_list;
  std::vector<f1ap_cell_to_bar>        cells_to_be_barred_list;

  std::string gnb_cu_name;

  // TODO: Add missing optional fields.
};

/// Common type F1AP GNB-CU Configuration Update Response. Success is true for a GNB-CU Configuration Update Acknowledge
/// (see TS 38.473, section 9.2.1.11), GNB-CU Configuration Update Failure (see TS 38.473, section 9.2.1.12) otherwise.
struct f1ap_gnb_cu_configuration_update_response {
  bool success = false;

  // GNB CU Configuration Update Acknowledge.
  std::vector<f1ap_cell_failed_to_activate> cells_failed_to_be_activated_list;

  // TODO: Add missing optional fields.

  // GNB CU Configuration Update Failure.
  std::optional<f1ap_cause_t> cause;

  // TODO: Add missing optional fields.
};

} // namespace ocucp
} // namespace ocudu
