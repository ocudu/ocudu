// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp_cho_types.h"
#include "cu_cp_types.h"
#include <optional>

namespace ocudu::ocucp {

struct cu_cp_intra_cu_handover_request {
  cu_cp_ue_index_t    source_ue_index = cu_cp_ue_index_t::invalid;
  cu_cp_du_index_t    target_du_index = cu_cp_du_index_t::invalid;
  nr_cell_global_id_t cgi;
  pci_t               target_pci = INVALID_PCI;
  /// When set, the request is treated as CHO candidate preparation (not immediate HO execution).
  std::optional<cu_cp_cho_preparation_request> cho_preparation;
};

struct cu_cp_intra_cu_handover_response {
  bool success = false;
  /// Present only for CHO preparation requests.
  std::optional<cu_cp_cho_preparation_result> cho_preparation_result;
};

} // namespace ocudu::ocucp
