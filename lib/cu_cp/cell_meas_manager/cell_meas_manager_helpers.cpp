// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_meas_manager_helpers.h"
#include "ocudu/adt/format.h"
#include <unordered_set>

using namespace ocudu;
using namespace ocucp;

/// Checks if the given NCI is in the NCIs span.
static bool is_nci_present(span<const nr_cell_identity> ncis, nr_cell_identity nci, ocudulog::basic_logger& logger)
{
  if (std::find(ncis.begin(), ncis.end(), nci) == ncis.end()) {
    return false;
  }

  auto msg = fmt::format("Cell {:#x} already present, but must be unique", nci);
  logger.error("{}", msg);
  fmt::println("CU-CP: {}", msg);
  return true;
}

/// Checks the validity of the given SSB frequency to MEAS configuration.
static bool
check_ssb_freq_to_meas_object(const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& ssb_freq_to_meas_object,
                              const serving_cell_meas_config&                             serving_cell_cfg,
                              ocudulog::basic_logger&                                     logger)
{
  if (ssb_freq_to_meas_object.empty()) {
    return true;
  }

  if (!serving_cell_cfg.ssb_arfcn.has_value()) {
    return true;
  }

  ssb_frequency_t ssb_freq    = serving_cell_cfg.ssb_arfcn->value();
  auto            meas_obj_it = ssb_freq_to_meas_object.find(ssb_freq);
  if (meas_obj_it == ssb_freq_to_meas_object.end()) {
    return true;
  }

  // Check if the measurement object is already present.
  if (rrc_meas_obj_nr meas_obj_nr = generate_measurement_object(serving_cell_cfg);
      is_duplicate(meas_obj_nr, meas_obj_it->second)) {
    return true;
  }

  // If a measurement object for this ssb_freq is already present but not an update, we reject the update.
  auto msg = fmt::format(
      "Measurement object for ssb_freq={} already exists, but has different ssb_scs, smtc1 and/or smtc2", ssb_freq);
  logger.error("{}", msg);
  fmt::println("CU-CP: {}", msg);

  return false;
}

/// Checks the validity of the periodic report configuration.
static bool check_periodic_report_configuration(std::optional<report_cfg_id_t> periodic_report_cfg_id,
                                                nr_cell_identity               nci,
                                                const std::map<report_cfg_id_t, rrc_report_cfg_nr>& report_config_ids,
                                                ocudulog::basic_logger&                             logger)
{
  if (!periodic_report_cfg_id.has_value()) {
    return true;
  }

  if (report_config_ids.find(*periodic_report_cfg_id) != report_config_ids.end()) {
    return true;
  }

  auto msg = fmt::format("Cell {:#x}: periodic report config id {} not found in configuration",
                         nci,
                         to_underlying(*periodic_report_cfg_id));
  logger.error("{}", msg);
  fmt::println("CU-CP: {}", msg);

  return false;
}

/// Checks the neighbor configuration validity.
static bool check_neighbor_configuration(span<const neighbor_cell_meas_config>               ncells,
                                         nr_cell_identity                                    nci,
                                         const std::map<report_cfg_id_t, rrc_report_cfg_nr>& report_config_ids,
                                         ocudulog::basic_logger&                             logger)
{
  for (const auto& ncell : ncells) {
    if (nci == ncell.nci) {
      auto msg = fmt::format("Cell {:#x} must not be its own neighbor", nci);
      logger.error("{}", msg);
      fmt::println("CU-CP: {}", msg);
      return false;
    }

    for (const auto& report_cfg_id : ncell.report_cfg_ids) {
      if (report_config_ids.find(report_cfg_id) == report_config_ids.end()) {
        auto msg = fmt::format("Cell {:#x}: report config id {} for neighbor {:#x} not found in configuration",
                               nci,
                               to_underlying(report_cfg_id),
                               ncell.nci);
        logger.error("{}", msg);
        fmt::println("CU-CP: {}", msg);
        return false;
      }
    }
  }

  return true;
}

void ocudu::ocucp::log_cells(const std::map<nr_cell_identity, cell_meas_config>& cells, ocudulog::basic_logger& logger)
{
  if (cells.empty()) {
    return;
  }

  logger.debug("Configured cells:");
  for (const auto& cell : cells) {
    logger.debug(" - {}", cell.second);
  }
}

bool ocudu::ocucp::is_complete(const serving_cell_meas_config& cfg)
{
  // All mandatory values must be present.
  if (!cfg.pci.has_value() || !cfg.band.has_value() || !cfg.ssb_mtc.has_value() || !cfg.ssb_arfcn.has_value() ||
      !cfg.ssb_scs.has_value()) {
    return false;
  }

  // Call validators of individual params.
  if (!is_scs_valid(*cfg.ssb_scs)) {
    return false;
  }

  // TODO: validate SSB ARFCN.
#ifdef SSB_ARFC_VALIDATOR
  if (error_type<std::string> ret = band_helper::is_dl_arfcn_valid_given_band(*cfg.band, *cfg.ssb_arfcn, *cfg.ssb_scs);
      !ret.has_value()) {
    ocudulog::fetch_basic_logger("CU-CP").error(
        "Invalid SSB ARFCN={} for band {}. Cause: {}", *cfg.ssb_arfcn, *cfg.band, ret.error());
    return false;
  }
#endif // SSB_ARFC_VALIDATOR

  return true;
}

bool ocudu::ocucp::is_valid_configuration(
    const cell_meas_manager_config&                             cfg,
    ocudulog::basic_logger&                                     logger,
    const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& ssb_freq_to_meas_object)
{
  std::vector<nr_cell_identity> ncis;
  // Verify neighbor cell lists: cell id must not be included in neighbor cell list.
  for (const auto& [nci, cell_cfg] : cfg.cells) {
    if (is_nci_present(ncis, nci, logger)) {
      return false;
    }

    ncis.push_back(nci);

    if (!check_ssb_freq_to_meas_object(ssb_freq_to_meas_object, cell_cfg.serving_cell_cfg, logger)) {
      return false;
    }

    if (!check_periodic_report_configuration(cell_cfg.periodic_report_cfg_id, nci, cfg.report_config_ids, logger)) {
      return false;
    }

    if (!check_neighbor_configuration(cell_cfg.ncells, nci, cfg.report_config_ids, logger)) {
      return false;
    }
  }

  return true;
}

bool ocudu::ocucp::is_complete(const cell_meas_manager_config& cfg, ocudulog::basic_logger& logger)
{
  if (!is_valid_configuration(cfg, logger)) {
    return false;
  }

  // Verify each neighbor cell has a valid config.
  for (const auto& cell : cfg.cells) {
    for (const auto& ncell : cell.second.ncells) {
      // Verify NCI is present.
      auto ncell_it = cfg.cells.find(ncell.nci);
      if (ncell_it == cfg.cells.end()) {
        logger.error("No config for cell id {} found", ncell.nci);
        return false;
      }

      // Verify the config for this cell is complete.
      if (!is_complete(ncell_it->second.serving_cell_cfg)) {
        logger.error("Measurement config for cell id {} is not complete", ncell.nci);
        return false;
      }
    }
  }

  return true;
}

void ocudu::ocucp::add_old_meas_config_to_rem_list(rrc_meas_cfg& new_cfg, const rrc_meas_cfg& old_cfg)
{
  // Remove measurement objects.
  for (const auto& meas_obj : old_cfg.meas_obj_to_add_mod_list) {
    new_cfg.meas_obj_to_rem_list.push_back(meas_obj.meas_obj_id);
  }

  // Remove measurement IDs.
  for (const auto& meas_id : old_cfg.meas_id_to_add_mod_list) {
    new_cfg.meas_id_to_rem_list.push_back(meas_id.meas_id);
  }

  // Remove active reports.
  for (const auto& report : old_cfg.report_cfg_to_add_mod_list) {
    new_cfg.report_cfg_to_rem_list.push_back(report.report_cfg_id);
  }
}

void ocudu::ocucp::prune_redundant_rem_list_entries(rrc_meas_cfg& new_cfg, const rrc_meas_cfg& old_cfg)
{
  // Reusing an id assumes the encoded measObject fully determines the entry: only its delta lists survive a
  // modification. cellsToAddMod is handled below; excludedCells/allowedCells are never populated. Populating those, or
  // adding a Need-M field, needs the same handling.
  std::vector<meas_obj_id_t>& meas_obj_rem_list = new_cfg.meas_obj_to_rem_list;
  for (auto& meas_obj : new_cfg.meas_obj_to_add_mod_list) {
    auto old_it = std::find_if(
        old_cfg.meas_obj_to_add_mod_list.begin(),
        old_cfg.meas_obj_to_add_mod_list.end(),
        [&meas_obj](const rrc_meas_obj_to_add_mod& old_obj) { return old_obj.meas_obj_id == meas_obj.meas_obj_id; });
    if (old_it == old_cfg.meas_obj_to_add_mod_list.end()) {
      continue;
    }

    meas_obj_rem_list.erase(std::remove(meas_obj_rem_list.begin(), meas_obj_rem_list.end(), meas_obj.meas_obj_id),
                            meas_obj_rem_list.end());

    // Cell lists are applied as a delta, so dropped cells must be removed explicitly.
    if (!old_it->meas_obj_nr.has_value() || !meas_obj.meas_obj_nr.has_value()) {
      continue;
    }
    rrc_meas_obj_nr& new_obj_nr = *meas_obj.meas_obj_nr;
    const auto&      new_cells  = new_obj_nr.cells_to_add_mod_list;
    for (const auto& old_cell : old_it->meas_obj_nr->cells_to_add_mod_list) {
      if (std::none_of(new_cells.begin(), new_cells.end(), [&old_cell](const rrc_cells_to_add_mod& new_cell) {
            return new_cell.pci == old_cell.pci;
          })) {
        new_obj_nr.cells_to_rem_list.push_back(old_cell.pci);
      }
    }
  }

  std::vector<report_cfg_id_t>& report_cfg_rem_list = new_cfg.report_cfg_to_rem_list;
  for (const auto& report_cfg : new_cfg.report_cfg_to_add_mod_list) {
    report_cfg_rem_list.erase(
        std::remove(report_cfg_rem_list.begin(), report_cfg_rem_list.end(), report_cfg.report_cfg_id),
        report_cfg_rem_list.end());
  }

  std::vector<meas_id_t>& meas_id_rem_list = new_cfg.meas_id_to_rem_list;
  for (const auto& meas_id : new_cfg.meas_id_to_add_mod_list) {
    meas_id_rem_list.erase(std::remove(meas_id_rem_list.begin(), meas_id_rem_list.end(), meas_id.meas_id),
                           meas_id_rem_list.end());
  }
}

std::vector<ssb_frequency_t> ocudu::ocucp::generate_measurement_object_list(nr_cell_identity                serving_nci,
                                                                            const cell_meas_manager_config& cfg,
                                                                            ocudulog::basic_logger&         logger)
{
  auto serving_it = cfg.cells.find(serving_nci);
  ocudu_assert(serving_it != cfg.cells.end(), "No cell config for nci={:#x}", serving_nci);

  // Add cells to lookup if report is configured.
  std::vector<ssb_frequency_t> ssb_freqs;

  // Add the serving cell frequency measurement object. Required for inter-frequency handovers (e.g. A3 HO).
  const cell_meas_config& serving_cell = serving_it->second;

  if (is_complete(serving_cell.serving_cell_cfg)) {
    ssb_freqs.push_back(serving_cell.serving_cell_cfg.ssb_arfcn->value());
  }

  // Add neighbor cells measurement objects if a non-conditional report is configured.
  for (const auto& ncell : serving_cell.ncells) {
    auto ncell_it = cfg.cells.find(ncell.nci);
    if (ncell_it == cfg.cells.end()) {
      logger.warning("No cell config for nci={:#x}, skipping", ncell.nci);
      continue;
    }
    const serving_cell_meas_config& ncell_cfg              = ncell_it->second.serving_cell_cfg;
    bool                            has_regular_report_cfg = std::any_of(
        ncell.report_cfg_ids.begin(), ncell.report_cfg_ids.end(), [&cfg](const report_cfg_id_t report_cfg_id) {
          return !is_cond_trigger_report_config(report_cfg_id, cfg.report_config_ids);
        });

    if (!has_regular_report_cfg || !is_complete(ncell_cfg)) {
      continue;
    }

    if (ssb_frequency_t ssb_freq = ncell_cfg.ssb_arfcn->value();
        std::find(ssb_freqs.begin(), ssb_freqs.end(), ssb_freq) == ssb_freqs.end()) {
      ssb_freqs.push_back(ssb_freq);
    }
  }

  return ssb_freqs;
}

std::vector<ssb_frequency_t>
ocudu::ocucp::generate_cho_measurement_object_list(nr_cell_identity                                    serving_nci,
                                                   span<const pci_t>                                   candidate_pcis,
                                                   const std::map<nr_cell_identity, cell_meas_config>& cells,
                                                   ocudulog::basic_logger&                             logger)
{
  auto serving_it = cells.find(serving_nci);
  ocudu_assert(serving_it != cells.end(), "No cell config for nci={:#x}", serving_nci);

  std::vector<ssb_frequency_t> freqs;

  // Add the serving cell frequency first, so that it gets the first measurement object id like in the regular
  // measurement configuration. The serving frequency is also required for inter-frequency CHO (e.g. A5).
  const cell_meas_config& serving_cell = serving_it->second;
  if (is_complete(serving_cell.serving_cell_cfg)) {
    freqs.push_back(serving_cell.serving_cell_cfg.ssb_arfcn->value());
  }

  std::unordered_set<pci_t> candidate_pcis_set(candidate_pcis.begin(), candidate_pcis.end());
  for (const auto& ncell : serving_cell.ncells) {
    auto ncell_it = cells.find(ncell.nci);
    if (ncell_it == cells.end()) {
      logger.warning("No cell config for nci={:#x}, skipping", ncell.nci);
      continue;
    }

    const serving_cell_meas_config& ncell_cfg = ncell_it->second.serving_cell_cfg;

    if (!is_complete(ncell_cfg) || !ncell_cfg.pci ||
        (!candidate_pcis_set.empty() && !candidate_pcis_set.count(*ncell_cfg.pci))) {
      continue;
    }

    if (ssb_frequency_t ssb_freq = ncell_cfg.ssb_arfcn->value();
        std::find(freqs.begin(), freqs.end(), ssb_freq) == freqs.end()) {
      freqs.push_back(ssb_freq);
    }
  }

  return freqs;
}

bool ocudu::ocucp::is_cond_trigger_report_config(report_cfg_id_t                                     report_cfg_id,
                                                 const std::map<report_cfg_id_t, rrc_report_cfg_nr>& report_config_ids)
{
  auto report_cfg_it = report_config_ids.find(report_cfg_id);
  return report_cfg_it != report_config_ids.end() &&
         std::holds_alternative<rrc_cond_trigger_cfg>(report_cfg_it->second);
}

void ocudu::ocucp::generate_report_config(rrc_meas_cfg&                   meas_cfg,
                                          cell_meas_manager_ue_context&   ue_meas_context,
                                          nr_cell_identity                nci,
                                          report_cfg_id_t                 report_cfg_id,
                                          const cell_meas_manager_config& cfg,
                                          ocudulog::basic_logger&         logger)
{
  // Add the report config to the add/mod list.
  auto report_cfg_it = cfg.report_config_ids.find(report_cfg_id);
  if (report_cfg_it == cfg.report_config_ids.end()) {
    logger.error("Report config ID {} not found in configuration", to_underlying(report_cfg_id));
    return;
  }

  meas_cfg.report_cfg_to_add_mod_list.push_back(
      rrc_report_cfg_to_add_mod{.report_cfg_id = report_cfg_id, .report_cfg = report_cfg_it->second});

  // Add meas id to link the cell and the report together.
  rrc_meas_id_to_add_mod meas_id_to_add_mod{.meas_id       = ue_meas_context.allocate_meas_id(),
                                            .meas_obj_id   = ue_meas_context.nci_to_meas_obj_id.at(nci),
                                            .report_cfg_id = report_cfg_id};
  meas_cfg.meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

  // Add meas id to lookup.
  const auto& serving_cell_cfg = cfg.cells.at(nci).serving_cell_cfg;
  ocudu_assert(serving_cell_cfg.pci.has_value(), "PCI must be set for nci={:#x}", nci);
  ue_meas_context.meas_id_to_meas_context.emplace(
      meas_id_to_add_mod.meas_id,
      meas_context_t{.meas_obj_id       = meas_id_to_add_mod.meas_obj_id,
                     .report_cfg_id     = meas_id_to_add_mod.report_cfg_id,
                     .gnb_id_bit_length = serving_cell_cfg.gnb_id_bit_length,
                     .nci               = serving_cell_cfg.nci,
                     .pci               = *serving_cell_cfg.pci});

  // Set T312 value in the measurement object if it's linked to (event-triggered) report config with T312 configured.
  if (const auto* event_triggered = std::get_if<rrc_event_trigger_cfg>(&report_cfg_it->second);
      event_triggered != nullptr && event_triggered->t312.has_value()) {
    auto meas_obj_it = std::find_if(meas_cfg.meas_obj_to_add_mod_list.begin(),
                                    meas_cfg.meas_obj_to_add_mod_list.end(),
                                    [&meas_id_to_add_mod](const rrc_meas_obj_to_add_mod& obj) {
                                      return obj.meas_obj_id == meas_id_to_add_mod.meas_obj_id;
                                    });
    if (meas_obj_it != meas_cfg.meas_obj_to_add_mod_list.end() && meas_obj_it->meas_obj_nr.has_value()) {
      meas_obj_it->meas_obj_nr->t312 = event_triggered->t312;
    }
  }
}

rrc_meas_obj_nr ocudu::ocucp::generate_measurement_object(const serving_cell_meas_config& cfg)
{
  // TODO: remove hardcoded values.
  static constexpr unsigned nof_ss_blocks_to_average = 8;
  static constexpr unsigned quant_cfg_idx            = 1;

  ocudu_assert(cfg.band.has_value(), "Band must be set to generate the measurement object of nci={:#x}", cfg.nci);

  std::optional<ssb_frequency_t> ssb_freq =
      cfg.ssb_arfcn.has_value() ? std::optional{cfg.ssb_arfcn->value()} : std::nullopt;

  // TODO: Add optional fields.
  return rrc_meas_obj_nr{.ssb_freq               = ssb_freq,
                         .ssb_subcarrier_spacing = cfg.ssb_scs,
                         .smtc1                  = cfg.ssb_mtc,
                         .smtc2                  = std::nullopt,
                         .ref_freq_csi_rs        = std::nullopt,
                         // Mandatory field.
                         .ref_sig_cfg = rrc_ref_sig_cfg{.ssb_cfg_mob = rrc_ssb_cfg_mob{.ssb_to_measure = std::nullopt,
                                                                                       .derive_ssb_idx_from_cell = true,
                                                                                       .ss_rssi_meas = std::nullopt},
                                                        .csi_rs_res_cfg_mob = std::nullopt},
                         .abs_thresh_ss_blocks_consolidation = std::nullopt,
                         .abs_thresh_csi_rs_consolidation    = std::nullopt,
                         // Mandatory field.
                         .nrof_ss_blocks_to_average  = nof_ss_blocks_to_average,
                         .nrof_csi_rs_res_to_average = std::nullopt,
                         // Mandatory field.
                         .quant_cfg_idx                  = quant_cfg_idx,
                         .offset_mo                      = rrc_q_offset_range_list{.rsrp_offset_ssb    = std::nullopt,
                                                                                   .rsrq_offset_ssb    = std::nullopt,
                                                                                   .sinr_offset_ssb    = std::nullopt,
                                                                                   .rsrp_offset_csi_rs = std::nullopt,
                                                                                   .rsrq_offset_csi_rs = std::nullopt,
                                                                                   .sinr_offset_csi_rs = std::nullopt},
                         .cells_to_rem_list              = {},
                         .cells_to_add_mod_list          = {},
                         .excluded_cells_to_rem_list     = {},
                         .excluded_cells_to_add_mod_list = {},
                         .allowed_cells_to_rem_list      = {},
                         .allowed_cells_to_add_mod_list  = {},
                         // Mandatory field.
                         .freq_band_ind_nr = to_underlying(*cfg.band),
                         .meas_cycle_scell = std::nullopt,
                         .t312             = std::nullopt};
}

bool ocudu::ocucp::is_duplicate(const rrc_meas_obj_nr& obj_1, const rrc_meas_obj_nr& obj_2)
{
  // TS38.331 Section 5.5.2.1:
  // For all SSB based measurements there is at most one measurement object with
  // the same ssbFrequency;
  // - an smtc1 included in any measurement object with the same ssbFrequency has the
  //   same value and that an smtc2 included in any measurement object with the
  //   same ssbFrequency has the same value;
  // - to ensure that all measurement objects configured in this specification and in
  //   TS 36.331 [10] with the same ssbFrequency have the same ssbSubcarrierSpacing;
  return obj_1.ssb_freq == obj_2.ssb_freq && obj_1.ssb_subcarrier_spacing == obj_2.ssb_subcarrier_spacing &&
         obj_1.smtc1 == obj_2.smtc1 && obj_1.smtc2 == obj_2.smtc2;
}

void ocudu::ocucp::log_meas_objects(const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& meas_objects,
                                    ocudulog::basic_logger&                                     logger)
{
  if (meas_objects.empty()) {
    return;
  }

  logger.debug("Measurement objects:");
  for (const auto& meas_obj : meas_objects) {
    logger.debug(" - ssb_freq={}: {}", meas_obj.first, meas_obj.second);
  }
}

std::vector<report_cfg_id_t>
ocudu::ocucp::collect_cond_trigger_report_configs(rrc_meas_cfg&                                       meas_cfg,
                                                  const rrc_ue_capability_handler&                    ue_caps,
                                                  const std::map<report_cfg_id_t, rrc_report_cfg_nr>& report_config_ids,
                                                  ocudulog::basic_logger&                             logger)
{
  // Maximum number of conditional trigger events, with and without support for condHO-TwoTriggerEvents-r16.
  static constexpr unsigned max_nof_cond_triggers_two_events = 2;
  static constexpr unsigned max_nof_cond_triggers_one_event  = 1;

  if (!ue_caps.is_conditional_handover_supported()) {
    logger.debug("UE does not support CHO (Rel-16); no conditional trigger configs will be added");
    return {};
  }
  bool     two_trigger_capable = ue_caps.is_conditional_handover_two_trigger_events_supported();
  unsigned max_configs = two_trigger_capable ? max_nof_cond_triggers_two_events : max_nof_cond_triggers_one_event;

  std::vector<report_cfg_id_t> cond_trigger_ids;
  for (const auto& [report_cfg_id, report_cfg] : report_config_ids) {
    if (cond_trigger_ids.size() >= max_configs) {
      break;
    }
    if (!std::holds_alternative<rrc_cond_trigger_cfg>(report_cfg)) {
      continue;
    }
    // Filter Rel-17 trigger types by UE capability.
    const auto&              cond_trigger = std::get<rrc_cond_trigger_cfg>(report_cfg);
    rrc_event_id::event_id_t event_id     = cond_trigger.cond_event_id.id;
    if (event_id == rrc_event_id::event_id_t::a4 && !ue_caps.is_conditional_handover_event_a4_supported()) {
      logger.debug("Skipping event-A4 CHO trigger (report_cfg_id={}): UE does not support eventA4BasedCondHandover-r17",
                   fmt::underlying(report_cfg_id));
      continue;
    }
    if (event_id == rrc_event_id::event_id_t::d1 && !ue_caps.is_conditional_handover_location_based_supported()) {
      logger.debug(
          "Skipping event-D1 CHO trigger (report_cfg_id={}): UE does not support locationBasedCondHandover-r17",
          fmt::underlying(report_cfg_id));
      continue;
    }
    if (event_id == rrc_event_id::event_id_t::t1 && !ue_caps.is_conditional_handover_time_based_supported()) {
      logger.debug("Skipping event-T1 CHO trigger (report_cfg_id={}): UE does not support timeBasedCondHandover-r17",
                   fmt::underlying(report_cfg_id));
      continue;
    }
    meas_cfg.report_cfg_to_add_mod_list.push_back(
        rrc_report_cfg_to_add_mod{.report_cfg_id = report_cfg_id, .report_cfg = report_cfg});
    cond_trigger_ids.push_back(report_cfg_id);
  }
  return cond_trigger_ids;
}

bool ocudu::ocucp::generate_cho_meas_ids(rrc_meas_cfg&                                       meas_cfg,
                                         cell_meas_manager_ue_context&                       ue_meas_context,
                                         span<const report_cfg_id_t>                         cond_trigger_ids,
                                         const std::map<nr_cell_identity, cell_meas_config>& cells)
{
  for (const auto& meas_obj : meas_cfg.meas_obj_to_add_mod_list) {
    for (const auto& report_cfg_id : cond_trigger_ids) {
      meas_id_t meas_id = ue_meas_context.allocate_meas_id();
      if (meas_id == meas_id_t::invalid) {
        return false;
      }

      meas_cfg.meas_id_to_add_mod_list.push_back(rrc_meas_id_to_add_mod{
          .meas_id = meas_id, .meas_obj_id = meas_obj.meas_obj_id, .report_cfg_id = report_cfg_id});

      for (const auto& [context_nci, context_meas_obj_id] : ue_meas_context.nci_to_meas_obj_id) {
        if (context_meas_obj_id != meas_obj.meas_obj_id) {
          continue;
        }
        const serving_cell_meas_config& cell_cfg = cells.at(context_nci).serving_cell_cfg;
        ocudu_assert(cell_cfg.pci.has_value(), "PCI must be set for CHO candidate nci={:#x}", context_nci);
        ue_meas_context.meas_id_to_meas_context.emplace(meas_id,
                                                        meas_context_t{.meas_obj_id       = meas_obj.meas_obj_id,
                                                                       .report_cfg_id     = report_cfg_id,
                                                                       .gnb_id_bit_length = cell_cfg.gnb_id_bit_length,
                                                                       .nci               = context_nci,
                                                                       .pci               = *cell_cfg.pci});
        meas_cfg.nci_to_meas_ids[context_nci].push_back(meas_id);
      }
    }
  }
  return true;
}
