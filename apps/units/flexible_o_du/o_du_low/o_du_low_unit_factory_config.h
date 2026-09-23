// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_low_hal_factory.h"
#include "ocudu/du/du_low/du_low_executor_mapper.h"
#include "ocudu/du/du_low/o_du_low_config.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include <chrono>

namespace ocudu {

struct du_low_unit_config;

/// O-RAN DU low unit configuration.
struct o_du_low_unit_config {
  struct du_low_config {
    odu::cell_prach_ports_entry            prach_ports;
    unsigned                               max_puschs_per_slot;
    frequency_range                        freq_range;
    duplex_mode                            duplex;
    subcarrier_spacing                     scs_common;
    unsigned                               bw_rb;
    std::optional<tdd_ul_dl_config_common> tdd_pattern;
    unsigned                               prach_config_index;
    unsigned                               pusch_max_nof_layers;
    unsigned                               nof_rx_antennas;
    unsigned                               nof_tx_antennas;
    antenna_topology                       tx_ant_topology = antenna_topology::one_port;
    /// NTN cell-specific K-offset. Empty for terrestrial cells.
    std::optional<std::chrono::milliseconds> ntn_cs_koffset;
    /// NTN k_mac offset. Empty for terrestrial cells and for NTN cells with aligned DL/UL frame timing at the gNB.
    std::optional<std::chrono::milliseconds> ntn_k_mac;
  };

  const du_low_unit_config&                      du_low_unit_cfg;
  std::vector<du_low_config>                     cells;
  fapi_adaptor::phy_fapi_fastpath_adaptor_config fapi_cfg;
};

/// O-RAN DU low unit dependencies.
struct o_du_low_unit_dependencies {
  upper_phy_rg_gateway&                 rg_gateway;
  upper_phy_rx_symbol_request_notifier& rx_symbol_request_notifier;
  odu::du_low_executor_mapper&          workers;
  task_executor&                        fapi_p5_executor;
};

} // namespace ocudu
