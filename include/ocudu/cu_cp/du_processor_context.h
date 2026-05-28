// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/tac.h"

namespace ocudu::ocucp {

/// Basic cell system information provided by DU
struct du_sys_info {
  byte_buffer packed_mib;
  byte_buffer packed_sib1;
};

struct du_cell_configuration {
  /// CU-CP specific DU cell identifier.
  cu_cp_du_cell_index_t cell_index = cu_cp_du_cell_index_t::invalid;
  /// Global cell ID.
  nr_cell_global_id_t cgi;
  /// Tracking Area Code
  tac_t tac;
  /// Physical cell ID
  pci_t pci;
  /// PLMN identitys served by the cell.
  std::vector<plmn_identity> served_plmns;
  /// NR bands provided/supported by the cell.
  std::vector<nr_band> bands;
  /// System Information provided by the DU for this cell.
  du_sys_info sys_info;
  /// Deactivated PLMN identitys initially served by the cell.
  std::vector<plmn_identity> deactivated_plmns;
};

} // namespace ocudu::ocucp
