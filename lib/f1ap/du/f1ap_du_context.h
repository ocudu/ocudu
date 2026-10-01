// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/ran/gnb_du_id.h"

namespace ocudu {
namespace odu {

struct f1ap_du_cell_context {
  // DU-specific cell index.
  du_cell_index_t cell_index;
  // Global cell ID.
  nr_cell_global_id_t nr_cgi;
  // NTN Link Round Trip Time.
  std::chrono::milliseconds ntn_link_rtt = std::chrono::milliseconds(0);
};

/// DU Context stored in the F1AP-DU. It includes information about the DU serving cells.
struct f1ap_du_context {
  gnb_du_id_t du_id = gnb_du_id_t::invalid;
  std::string gnb_du_name;
  bool        f1c_setup = false;
  /// Last served cells reported to the CU-CP.
  slotted_id_vector<du_cell_index_t, f1ap_du_cell_context> served_cells;

  /// Fetch cell context with matching CGI.
  const f1ap_du_cell_context* find_cell(nr_cell_global_id_t cgi) const
  {
    auto it = std::find_if(
        served_cells.begin(), served_cells.end(), [&cgi](const f1ap_du_cell_context& c) { return c.nr_cgi == cgi; });
    return it != served_cells.end() ? &*it : nullptr;
  }
};

} // namespace odu
} // namespace ocudu
