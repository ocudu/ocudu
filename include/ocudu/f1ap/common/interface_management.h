// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cause/f1ap_cause.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include <optional>

namespace ocudu {

/// Cell commanded by the CU to be activated via the F1AP interface.
struct f1ap_cell_to_activate {
  nr_cell_global_id_t        cgi;
  std::optional<pci_t>       pci;
  std::vector<plmn_identity> available_plmn_list;
};

/// Cell failed to be activated via the F1AP interface.
struct f1ap_cell_failed_to_activate {
  nr_cell_global_id_t cgi;
  f1ap_cause_t        cause;
};

/// Cell commanded by the CU to be deactivated via the F1AP interface.
struct f1ap_cell_to_deactivate {
  nr_cell_global_id_t cgi;
};

/// Cell commanded by the CU to be barred/unbarred via the F1AP interface, see TS 38.473 Cells to be Barred List.
struct f1ap_cell_to_bar {
  nr_cell_global_id_t cgi;
  /// New MIB cellBarred state for the cell: true for barred, false for notBarred (TS 38.331).
  bool barred = true;
};

} // namespace ocudu
