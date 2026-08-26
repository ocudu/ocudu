// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/du_cell_index.h"
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
  du_cell_index_t cell_index = INVALID_DU_CELL_INDEX;
  /// Global cell ID.
  nr_cell_global_id_t cgi;
  /// Tracking Area Code. Invalid if the gNB-DU did not provide the 5GS TAC of the cell.
  tac_t tac = INVALID_TAC;
  /// \brief TACs the cell broadcasts in \c trackingAreaList, TS 38.331. Empty unless several.
  ///
  /// Read back from the gNB-DU's SIB1, so it matches what the cell broadcasts, in broadcast order.
  tac_list_t tac_list;
  /// Physical cell ID
  pci_t pci;
  /// PLMN identitys served by the cell.
  std::vector<plmn_identity> served_plmns;
  /// NR bands provided/supported by the cell.
  std::vector<nr_band> bands;
  /// Duplex mode and carrier frequencies of the cell.
  cu_cp_nr_mode_info nr_mode_info;
  /// MeasurementTimingConfiguration of the cell, as defined in TS 38.331 section 6.2.2.
  byte_buffer meas_timing_cfg;
  /// System Information provided by the DU for this cell.
  du_sys_info sys_info;
  /// Deactivated PLMN identitys initially served by the cell.
  std::vector<plmn_identity> deactivated_plmns;
};

} // namespace ocudu::ocucp
