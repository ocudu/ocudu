// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include <map>

using namespace ocudu;

unsigned
ocudu::config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(const serving_cell_config& ue_cell_cfg,
                                                                           const dl_config_common&    dl_cfg_common)
{
  if (not ue_cell_cfg.init_dl_bwp.pdcch_cfg.has_value()) {
    return 0;
  }
  return compute_tot_nof_monitored_pdcch_candidates_per_slot(*ue_cell_cfg.init_dl_bwp.pdcch_cfg, dl_cfg_common);
}

unsigned
ocudu::config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(const pdcch_config&     ded_pdcch_cfg,
                                                                           const dl_config_common& dl_cfg_common)
{
  // NOTE: We assume DCI formats other than 1_0, 0_0, 1_1 and 0_1 are not configured in SearchSpaces.
  // NOTE: Total nof. monitored PDCCH candidates are calculated considering a slot at which all SearchSpaces are active
  // for simplification.

  // As per TS 38.213, clause 10.1, "A UE expects to monitor PDCCH candidates for up to 4 sizes of DCI formats that
  // include up to 3 sizes of DCI formats with CRC scrambled by C-RNTI per serving cell. The UE counts a number of sizes
  // for DCI formats per serving cell based on a number of configured PDCCH candidates in respective search space sets
  // for the corresponding active DL BWP".

  // As per TS 38.212, clause 7.3.1.2.1, "If DCI format 1_0 is monitored in UE specific search space and satisfies both
  // of the following
  // - the total number of different DCI sizes monitored per slot is no more than 4 for the cell, and
  // - the total number of different DCI sizes with C-RNTI monitored per slot is no more than 3 for the cell
  // and if the number of information bits in the DCI format 1_0 prior to padding is less than the payload size of the
  // DCI format 0_0 monitored in UE specific search space for scheduling the same serving cell, zeros shall be appended
  // to the DCI format 1_0 until the payload size equals that of the DCI format 0_0".

  // NOTE: After performing the DCI size alignment mentioned in TS 38.212 (release 16), clause 7.3.1.0. The size of DCI
  // 1_0 scrambled by C-RNTI, TC-RNTI, P-RNTI, SI-RNTI and RA-RNTI will be same. And, the size of DCI 0_0 scrambled by
  // C-RNTI will be same as size of DCI 1_0 scrambled by C-RNTI.
  static constexpr unsigned nof_fallback_dci_sizes_to_be_monitored = 1;
  // UE needs to monitor PDCCH candidates for both DCI 1_1 and 0_1, which have different sizes.
  static constexpr unsigned nof_non_fallback_dci_sizes_to_be_monitored_in_uss = 2;

  std::map<search_space_id, unsigned> nof_monitored_pdcch_candidates_per_ss;

  const bwp_downlink_common& bwp_cmn = dl_cfg_common.init_dl_bwp;

  for (const search_space_configuration& ss : ded_pdcch_cfg.search_spaces) {
    const auto& dci_format_variant = ss.get_monitored_dci_formats();
    const bool  non_fallback_dci_fmt =
        std::holds_alternative<search_space_configuration::ue_specific_dci_format>(dci_format_variant) and
        std::get<search_space_configuration::ue_specific_dci_format>(dci_format_variant) ==
            search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;

    unsigned nof_monitored_pdcch_candidates;
    if (ss.is_common_search_space() or not non_fallback_dci_fmt) {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_fallback_dci_sizes_to_be_monitored;
    } else {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_non_fallback_dci_sizes_to_be_monitored_in_uss;
    }

    for (const auto& entry : nof_monitored_pdcch_candidates_per_ss) {
      const auto it = std::find_if(
          ded_pdcch_cfg.search_spaces.begin(),
          ded_pdcch_cfg.search_spaces.end(),
          [ss_id = entry.first](const search_space_configuration& ss_cfg) { return ss_cfg.get_id() == ss_id; });
      // [Implementation-defined] Reset nof. monitored PDCCH candidates for earlier accounted SearchSpace so that we
      // account only the highest nof. monitored PDCCH candidates for SeachSpaces sharing the same CORESET and
      // same DCI formats. This is done to simplify calculation of total nof. PDCCH candidates monitored across all
      // SearchSpaces.
      // For example: Assume SS#2 and SS#3 share CORESET#1 and monitoring DCI format 1_0/0_0. And, nof.
      // PDCCH candidates to monitor is 4 and 12 respectively. In this case, we consider only 12 PDCCH candidates to
      // monitor.
      if (it != ded_pdcch_cfg.search_spaces.end() and it->get_coreset_id() == ss.get_coreset_id() and
          it->get_monitored_dci_formats() == ss.get_monitored_dci_formats()) {
        if (nof_monitored_pdcch_candidates > nof_monitored_pdcch_candidates_per_ss[entry.first]) {
          nof_monitored_pdcch_candidates_per_ss[entry.first] = 0;
          break;
        }
        nof_monitored_pdcch_candidates = 0;
      }
    }
    nof_monitored_pdcch_candidates_per_ss[ss.get_id()] = nof_monitored_pdcch_candidates;
  }

  for (const search_space_configuration& ss : bwp_cmn.pdcch_common.search_spaces) {
    const auto& dci_format_variant = ss.get_monitored_dci_formats();
    const bool  non_fallback_dci_fmt =
        std::holds_alternative<search_space_configuration::ue_specific_dci_format>(dci_format_variant) and
        std::get<search_space_configuration::ue_specific_dci_format>(dci_format_variant) ==
            search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;

    unsigned nof_monitored_pdcch_candidates;
    if (ss.is_common_search_space() or not non_fallback_dci_fmt) {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_fallback_dci_sizes_to_be_monitored;
    } else {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_non_fallback_dci_sizes_to_be_monitored_in_uss;
    }

    for (const auto& entry : nof_monitored_pdcch_candidates_per_ss) {
      const auto it = std::find_if(
          ded_pdcch_cfg.search_spaces.begin(),
          ded_pdcch_cfg.search_spaces.end(),
          [ss_id = entry.first](const search_space_configuration& ss_cfg) { return ss_cfg.get_id() == ss_id; });
      // [Implementation-defined] Reset nof. monitored PDCCH candidates for earlier accounted SearchSpace so that we
      // account only the highest nof. monitored PDCCH candidates for SeachSpaces sharing the same CORESET and
      // same DCI formats. This is done to simplify calculation of total nof. PDCCH candidates monitored across all
      // SearchSpaces.
      // For example: Assume SS#2 and SS#3 share CORESET#1 and monitoring DCI format 1_0/0_0. And, nof.
      // PDCCH candidates to monitor is 4 and 12 respectively. In this case, we consider only 12 PDCCH candidates to
      // monitor.
      if (it != ded_pdcch_cfg.search_spaces.end() and it->get_coreset_id() == ss.get_coreset_id() and
          it->get_monitored_dci_formats() == ss.get_monitored_dci_formats()) {
        if (nof_monitored_pdcch_candidates > nof_monitored_pdcch_candidates_per_ss[entry.first]) {
          nof_monitored_pdcch_candidates_per_ss[entry.first] = 0;
          break;
        }
        nof_monitored_pdcch_candidates = 0;
      }
      const auto cmn_it = std::find_if(
          bwp_cmn.pdcch_common.search_spaces.begin(),
          bwp_cmn.pdcch_common.search_spaces.end(),
          [ss_id = entry.first](const search_space_configuration& ss_cfg) { return ss_cfg.get_id() == ss_id; });
      if (cmn_it != bwp_cmn.pdcch_common.search_spaces.end() and cmn_it->get_coreset_id() == ss.get_coreset_id() and
          cmn_it->get_monitored_dci_formats() == ss.get_monitored_dci_formats()) {
        if (nof_monitored_pdcch_candidates > nof_monitored_pdcch_candidates_per_ss[entry.first]) {
          nof_monitored_pdcch_candidates_per_ss[entry.first] = 0;
          break;
        }
        nof_monitored_pdcch_candidates = 0;
      }
    }
    nof_monitored_pdcch_candidates_per_ss[ss.get_id()] = nof_monitored_pdcch_candidates;
  }

  unsigned total_nof_monitored_pdcch_candidates = 0;
  for (const auto& entry : nof_monitored_pdcch_candidates_per_ss) {
    total_nof_monitored_pdcch_candidates += entry.second;
  }

  return total_nof_monitored_pdcch_candidates;
}
