// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/dmrs/dmrs_downlink_config.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/result/dmrs_info.h"

namespace ocudu {

class cell_configuration;

struct pdsch_config_params {
  pdsch_mcs_table   mcs_table;
  ofdm_symbol_range symbols;
  unsigned          nof_oh_prb;
  unsigned          tb_scaling_field;
  dmrs_information  dmrs;
  unsigned          max_nof_cws_scheduled_by_dci;

  /// Returns the number of layers of the PDSCH, which transmits one layer per DM-RS port (see TS 38.211,
  /// Section 7.3.1.4).
  unsigned nof_layers() const { return dmrs.dmrs_ports.count(); }
};

namespace sched_helper {

/// \brief Fetches the PDSCH parameters needed for PDSCH PDU for DCI format 1_0, scrambled by TC-RNTI.
///
/// Returns parameters needed to compute the number of PRBs, MCS and TBS.
pdsch_config_params get_pdsch_config_f1_0_tc_rnti(const cell_configuration&                    cell_cfg_common,
                                                  const pdsch_time_domain_resource_allocation& pdsch_td_cfg);

/// \brief Fetches the PDSCH parameters needed for PUSCH PDU for DCI format 1_0, scrambled by C-RNTI.
///
/// Returns parameters needed to compute the number of PRBs, MCS and TBS.
pdsch_config_params get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg_common,
                                                 const pdsch_time_domain_resource_allocation& pdsch_td_cfg);
/// Same as above, for a UE that assumes \c nof_oh_prb PRBs of overhead.
pdsch_config_params get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg_common,
                                                 unsigned                                     nof_oh_prb,
                                                 const pdsch_time_domain_resource_allocation& pdsch_td_cfg);

/// \brief Fetches the PDSCH parameters needed for PUSCH PDU for DCI format 1_1, scrambled by C-RNTI.
///
/// Returns parameters needed to compute the number of PRBs, MCS and TBS.
pdsch_config_params get_pdsch_config_f1_1_c_rnti(const cell_configuration&                    cell_cfg_common,
                                                 const dmrs_downlink_config&                  dmrs_cfg,
                                                 pdsch_mcs_table                              mcs_table,
                                                 unsigned                                     nof_oh_prb,
                                                 const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                                 unsigned                                     nof_layers);

} // namespace sched_helper
} // namespace ocudu
