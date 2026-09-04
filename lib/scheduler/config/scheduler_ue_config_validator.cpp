// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/scheduler_ue_config_validator.h"
#include "cell_configuration.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/ran/configured_grant/cg_configuration.h"
#include "ocudu/scheduler/config/serving_cell_config_validator.h"
#include "ocudu/support/config/validator_helpers.h"

using namespace ocudu;
using namespace config_validators;

static validator_result validate_cg_cfg(const uplink_config& ul_cfg, const cell_configuration& cell_cfg)
{
  const auto& bwp_ul_ded_cfg = ul_cfg.init_ul_bwp;
  VERIFY(bwp_ul_ded_cfg.cg_cfg.has_value(), "Configured Grant configuration not set");
  const auto& cg_cfg = bwp_ul_ded_cfg.cg_cfg.value();

  // NOTE: the actual HARQ timeout (in slots) is the configured_grant_timer multiplied by the CG period in slot.
  VERIFY(cg_configuration::configured_grant_timer <= cg_cfg.nof_harq_processes,
         "configured_grant_timer ({}) must be <= nof_harq_processes ({}) so the CG HARQ timeout expires before the "
         "next CG occasion for the same HARQ ID",
         cg_configuration::configured_grant_timer,
         cg_cfg.nof_harq_processes);

  // The CG scheduler requires all UL HARQ processes to operate in mode A, i.e. \c ul_harq_mode must keep its default
  // value (all-ones mask).
  VERIFY(not ul_cfg.pusch_serv_cell_cfg.has_value() or
             ul_cfg.pusch_serv_cell_cfg->ul_harq_mode == ~harq_ul_mode_mask(MAX_NOF_HARQS),
         "Configured Grant requires the default uplinkHARQ-mode (all UL HARQ processes in mode A)");

  if (cell_cfg.params.tdd_cfg.has_value()) {
    // If every full UL slot is a CG opportunity, the scheduler couldn't allocate any dynamic PUSCHs, which is necessary
    // to complete the RRC reconfiguration.
    const unsigned nof_tdd_slots_per_period     = nof_slots_per_tdd_period(cell_cfg.params.tdd_cfg.value());
    const unsigned nof_full_ul_slots_per_period = nof_full_ul_slots_per_tdd_period(cell_cfg.params.tdd_cfg.value());
    VERIFY(nof_tdd_slots_per_period < static_cast<unsigned>(cg_cfg.periodicity) or
               (nof_tdd_slots_per_period == static_cast<unsigned>(cg_cfg.periodicity) and
                nof_full_ul_slots_per_period > 1U),
           "TDD configuration is not compatible with CG period");
  }

  const bool type2 = cell_cfg.params.init_bwp.cg_cfg.has_value() and cell_cfg.params.init_bwp.cg_cfg->is_type2();
  VERIFY(cg_cfg.rrc_configured_ul_grant_cfg.has_value() != type2,
         "Configured Grant type {} requires rrc_configured_ul_grant to be {}",
         type2 ? 2 : 1,
         type2 ? "absent" : "present");

  if (type2) {
    return {};
  }

  const auto& cg_ul_grant = cg_cfg.rrc_configured_ul_grant_cfg.value();

  // TS 38.214, Section 6.1.2.3: a Type 1 CG with PUSCH repetition Type A selects its TDRA table as for DCI format 0_0
  // in a UE-specific search space, i.e. the UE's dedicated pusch-TimeDomainAllocationList when configured, and the
  // common one otherwise (Table 6.1.2.1.1-1).
  const bool uses_ded_pusch_td_list =
      bwp_ul_ded_cfg.pusch_cfg.has_value() and not bwp_ul_ded_cfg.pusch_cfg->pusch_td_alloc_list.empty();
  const span<const pusch_time_domain_resource_allocation> pusch_td_list =
      uses_ded_pusch_td_list
          ? span<const pusch_time_domain_resource_allocation>(bwp_ul_ded_cfg.pusch_cfg->pusch_td_alloc_list)
          : span<const pusch_time_domain_resource_allocation>(cell_cfg.init_bwp.ul.pusch_common()->pusch_td_alloc_list);
  VERIFY(cg_ul_grant.time_domain_allocation < pusch_td_list.size(),
         "Configured Grant time domain allocation index ({}) is out of range for the {} PUSCH time domain allocation "
         "list ({} entries)",
         cg_ul_grant.time_domain_allocation,
         uses_ded_pusch_td_list ? "dedicated" : "common",
         pusch_td_list.size());

  // The CG scheduler emits one PUSCH per occasion, so the selected row must not request slot repetitions. Reachable
  // only through a dedicated Rel-16 TDRA list, whose rows carry numberOfRepetitions-r16.
  VERIFY(pusch_td_list[cg_ul_grant.time_domain_allocation].nof_repetitions.value_or(1) == 1,
         "Configured Grant time domain allocation index ({}) points at a TDRA row requesting {} PUSCH repetitions, "
         "which the Configured Grant scheduler does not support",
         cg_ul_grant.time_domain_allocation,
         pusch_td_list[cg_ul_grant.time_domain_allocation].nof_repetitions.value_or(1));

  return {};
}

validator_result validate_bwp_ded_cfg(const serving_cell_config& ue_cell_cfg, const cell_configuration& cell_cfg)
{
  VERIFY(ue_cell_cfg.dl_bwps.empty(), "Only init DL BWP is supported");
  const auto& ue_bwp_ded              = ue_cell_cfg.init_dl_bwp;
  const auto& expected_bwp_ded_pdcchs = cell_cfg.bwp_res[to_bwp_id(0)].dl().ded_pdcchs;
  if (ue_bwp_ded.pdcch_cfg.has_value()) {
    // If UE has been provided a dedicated PDCCH config, verify it matches one of the base ones.
    auto pdcch_it =
        std::find(expected_bwp_ded_pdcchs.begin(), expected_bwp_ded_pdcchs.end(), ue_bwp_ded.pdcch_cfg.value());
    if (pdcch_it == expected_bwp_ded_pdcchs.end()) {
      return make_unexpected("Inconsistent PDCCH config");
    }
  }
  return {};
}

error_type<std::string>
ocudu::config_validators::validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg,
                                                                     const cell_configuration&                cell_cfg)
{
  // Verify the list of ServingCellConfig contains spCellConfig.
  VERIFY(msg.cfg.cells.has_value() and not msg.cfg.cells->empty(), "Empty list of ServingCellConfig");

  for (const ue_cell_config& cell : *msg.cfg.cells) {
    const auto& serv_cell_cfg = cell.serv_cell_cfg;
    HANDLE_ERROR(validate_pdcch_cfg(serv_cell_cfg, cell_cfg.params.dl_cfg_common));
    HANDLE_ERROR(validate_bwp_ded_cfg(serv_cell_cfg, cell_cfg));

    HANDLE_ERROR(validate_pdsch_cfg(serv_cell_cfg));

    if (serv_cell_cfg.ul_config.has_value()) {
      if (serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.has_value() and
          serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value() and
          cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common.has_value()) {
        const pucch_config_common& pucch_cfg_common =
            cell_cfg.params.ul_cfg_common.init_ul_bwp.pucch_cfg_common.value();
        HANDLE_ERROR(validate_pucch_cfg(serv_cell_cfg,
                                        cell_cfg.params.init_bwp.pucch.resources,
                                        cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.dedicated,
                                        pucch_cfg_common,
                                        cell_cfg.params.dl_carrier.nof_ant));
        HANDLE_ERROR(validate_srs_cfg(serv_cell_cfg, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs));
      }

      HANDLE_ERROR(validate_pusch_cfg(serv_cell_cfg.ul_config.value(), serv_cell_cfg.csi_meas_cfg.has_value()));

      if (serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg.has_value()) {
        HANDLE_ERROR(validate_cg_cfg(serv_cell_cfg.ul_config.value(), cell_cfg));
      }
    }

    HANDLE_ERROR(validate_csi_meas_cfg(serv_cell_cfg, cell_cfg.params.tdd_cfg, cell_cfg.params.ul_cfg_common));

    // At the moment, we only support the situation where all UEs have the same NZP-CSI-RS list.
    if (serv_cell_cfg.csi_meas_cfg.has_value() and not serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list.empty()) {
      VERIFY(serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list == cell_cfg.nzp_csi_rs_list,
             "The NZP-CSI-RS Resource lists for the UE and the cell do not match");
    }
  }

  // TODO: Validate other parameters.
  return {};
}
