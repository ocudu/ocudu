// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_cell_params.h"
#include "ocudu/ran/carrier_configuration.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/n_ta_offset.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/bwp_builder_params.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include <optional>

namespace ocudu {

/// RAN-specific parameters of a given serving cell.
struct ran_cell_config {
  /// DL carrier configuration.
  carrier_configuration dl_carrier;
  /// UL carrier configuration.
  carrier_configuration ul_carrier;
  /// PCI of the cell.
  pci_t pci;
  /// SSB configuration.
  ssb_configuration ssb_cfg;
  /// Position of first DM-RS in Downlink, as per TS 38.211, 7.4.1.1.1.
  dmrs_typeA_position dmrs_typeA_pos;
  /// Cell-specific DL configuration used by common searchSpaces.
  dl_config_common dl_cfg_common;
  /// Cell-specific UL configuration used by common searchSpaces.
  ul_config_common ul_cfg_common;
  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
  /// Parameters for the initial UE-dedicated BWP config generation.
  bwp_builder_params init_bwp;
  /// NTN configuration for this cell. When empty, the cell operates in terrestrial mode.
  std::optional<ntn_cell_params> ntn_params;
  /// Cell time advance offset - parameter \f$N_{TA,offset}\f$ in TS38.211 Section 4.3.3.
  n_ta_offset ta_offset = n_ta_offset::n0;
};

} // namespace ocudu
