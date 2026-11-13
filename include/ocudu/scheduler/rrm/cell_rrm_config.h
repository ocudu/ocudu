// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/scheduler/rrm/cell_pucch_resources.h"

namespace ocudu {

struct ran_cell_config;

/// PUCCH resources that the RRM allocates to the UEs of a cell.
struct cell_pucch_rrm_config {
  /// UE-dedicated PUCCH resources of the cell, and the map from the IDs that the RRM allocates to a UE to them.
  cell_pucch_resources res;
  /// Periodicity of the SR.
  sr_periodicity sr_period = sr_periodicity::sl_40;
  /// Maximum code rate of the Format 2, 3 and 4 resources.
  max_pucch_code_rate max_code_rate_234 = max_pucch_code_rate::not_set;
  /// Slot offsets between a PDSCH and its HARQ-ACK.
  static_vector<uint8_t, 8> dl_data_to_ul_ack;
};

/// Cell resources that the RRM allocates to the UEs of a cell.
struct cell_rrm_config {
  cell_pucch_rrm_config pucch;
};

/// Generates the cell resources that the RRM allocates to the UEs of a cell.
cell_rrm_config make_cell_rrm_config(const ran_cell_config& cell_cfg);

} // namespace ocudu
