// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_config_params.h"
#include "../dmrs_helpers.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace sched_helper;

pdsch_config_params
sched_helper::get_pdsch_config_f1_0_tc_rnti(const cell_configuration&                    cell_cfg_common,
                                            const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  static constexpr pdsch_mcs_table mcs_table = pdsch_mcs_table::qam64;
  // As per TS 38.214, Section 5.1.3.2, TB scaling field can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb by \c xOverhead, defined in \c PDSCH-ServingCellConfig, TS 38.331; it
  // is in the dedicated resources, configured the after DCI Format 1-0 TC-RNTI is used. Hence, nof_oh_prb is here set
  // as 0.
  static constexpr unsigned nof_oh_prb = 0;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(pdsch_td_cfg, cell_cfg_common.params.pci, cell_cfg_common.params.dmrs_typeA_pos);

  pdsch.nof_oh_prb                   = nof_oh_prb;
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}

pdsch_config_params
sched_helper::get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg_common,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned        tb_scaling_field = 0;
  static constexpr pdsch_mcs_table mcs_table        = pdsch_mcs_table::qam64;

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_common(pdsch_td_cfg, cell_cfg_common.params.pci, cell_cfg_common.params.dmrs_typeA_pos);
  // NOTE: x_overhead::not_set is mapped to 0.
  pdsch.nof_oh_prb                   = static_cast<unsigned>(0);
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}

pdsch_config_params
sched_helper::get_pdsch_config_f1_0_c_rnti(const cell_configuration&                    cell_cfg_common,
                                           unsigned                                     nof_oh_prb,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg)
{
  pdsch_config_params params = get_pdsch_config_f1_0_c_rnti(cell_cfg_common, pdsch_td_cfg);
  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  params.nof_oh_prb = nof_oh_prb;
  return params;
}

pdsch_config_params
sched_helper::get_pdsch_config_f1_1_c_rnti(const cell_configuration&                    cell_cfg_common,
                                           dmrs_additional_positions                    dmrs_add_pos,
                                           dmrs_max_length                              dmrs_max_len,
                                           pdsch_mcs_table                              mcs_table,
                                           unsigned                                     nof_oh_prb,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                           unsigned                                     nof_layers)
{
  // As per TS 38.214, Section 5.1.3.2, TB scaling filed can be different to 0 only for DCI 1_0 with P-RNTI, or RA-RNTI.
  static constexpr unsigned tb_scaling_field = 0;

  ocudu_assert(nof_layers <= cell_cfg_common.params.dl_carrier.nof_ant,
               "Number of DL layers={} cannot be greater than number of DL antenna ports={} of the cell",
               nof_layers,
               cell_cfg_common.params.dl_carrier.nof_ant);

  pdsch_config_params pdsch;

  pdsch.dmrs = make_dmrs_info_dedicated(pdsch_td_cfg,
                                        cell_cfg_common.params.pci,
                                        cell_cfg_common.params.dmrs_typeA_pos,
                                        dmrs_add_pos,
                                        dmrs_max_len,
                                        get_su_pdsch_dmrs_ports(nof_layers));

  // According to TS 38.214, Section 5.1.3.2, nof_oh_prb is set equal to xOverhead, when set; else nof_oh_prb = 0.
  pdsch.nof_oh_prb                   = nof_oh_prb;
  pdsch.symbols                      = pdsch_td_cfg.symbols;
  pdsch.mcs_table                    = mcs_table;
  pdsch.tb_scaling_field             = tb_scaling_field;
  pdsch.max_nof_cws_scheduled_by_dci = 1;

  return pdsch;
}
