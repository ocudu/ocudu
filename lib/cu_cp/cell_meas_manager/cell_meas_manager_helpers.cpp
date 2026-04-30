// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_meas_manager_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <unordered_set>

using namespace ocudu;
using namespace ocucp;

#define LOG_CHAN ("CU-CP")

void ocudu::ocucp::log_cells(const ocudulog::basic_logger& logger, const cell_meas_manager_cfg& cfg)
{
  if (!cfg.cells.empty()) {
    logger.debug("Configured cells:");
    for (const auto& cell : cfg.cells) {
      logger.debug(" - {}", cell.second);
    }
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
  if (!is_scs_valid(cfg.ssb_scs.value())) {
    return false;
  }

  // TODO: validate ssb arfcn
#ifdef SSB_ARFC_VALIDATOR
  error_type<std::string> ret =
      band_helper::is_dl_arfcn_valid_given_band(cfg.band.value(), cfg.ssb_arfcn.value(), cfg.ssb_scs.value());
  if (not ret.has_value()) {
    ocudulog::fetch_basic_logger(LOG_CHAN).error(
        "Invalid SSB ARFCN={} for band {}. Cause: {}", cfg.ssb_arfcn.value(), cfg.band.value(), ret.error());
    return false;
  }
#endif // SSB_ARFC_VALIDATOR

  return true;
}

bool ocudu::ocucp::is_valid_configuration(
    const cell_meas_manager_cfg&                                cfg,
    const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& ssb_freq_to_meas_object)
{
  std::vector<nr_cell_identity> ncis;
  // Verify neighbor cell lists: cell id must not be included in neighbor cell list.
  for (const auto& cell : cfg.cells) {
    const auto& nci = cell.first;
    if (std::find(ncis.begin(), ncis.end(), nci) != ncis.end()) {
      ocudulog::fetch_basic_logger(LOG_CHAN).error("Cell {:#x} already present, but must be unique", nci);
      return false;
    }
    ncis.push_back(nci);

    if (!ssb_freq_to_meas_object.empty()) {
      const auto& serving_cell_cfg = cell.second.serving_cell_cfg;
      if (serving_cell_cfg.ssb_arfcn.has_value()) {
        ssb_frequency_t ssb_freq = serving_cell_cfg.ssb_arfcn.value().value();
        if (ssb_freq_to_meas_object.find(ssb_freq) != ssb_freq_to_meas_object.end()) {
          // Check if the measurement object is already present.
          rrc_meas_obj_nr meas_obj_nr = generate_measurement_object(serving_cell_cfg);
          if (!is_duplicate(meas_obj_nr, ssb_freq_to_meas_object.at(ssb_freq))) {
            // If a measurement object for this ssb_freq is already present but not an update, we reject the update.
            ocudulog::fetch_basic_logger(LOG_CHAN).error(
                "Measurement object for ssb_freq={} already exists, but has different ssb_scs, smtc1 and/or smtc2",
                ssb_freq);
            return false;
          }
        }
      }
    }

    for (const auto& ncell_nci : cell.second.ncells) {
      if (nci == ncell_nci.nci) {
        ocudulog::fetch_basic_logger(LOG_CHAN).error("Cell {:#x} must not be its own neighbor", nci);
        return false;
      }
    }
  }

  return true;
}

bool ocudu::ocucp::is_complete(const cell_meas_manager_cfg& cfg)
{
  if (!is_valid_configuration(cfg)) {
    return false;
  }

  // Verify each neighbor cell has a valid config.
  for (const auto& cell : cfg.cells) {
    for (const auto& ncell : cell.second.ncells) {
      // Verify NCI is present.
      if (cfg.cells.find(ncell.nci) == cfg.cells.end()) {
        ocudulog::fetch_basic_logger(LOG_CHAN).error("No config for cell id {} found", ncell.nci);
        return false;
      }

      // Verify the config for this cell is complete.
      if (!is_complete(cfg.cells.at(ncell.nci).serving_cell_cfg)) {
        ocudulog::fetch_basic_logger(LOG_CHAN).error("Measurement config for cell id {} is not complete", ncell.nci);
        return false;
      }
    }
  }

  return true;
}

void ocudu::ocucp::add_old_meas_config_to_rem_list(const rrc_meas_cfg& old_cfg, rrc_meas_cfg& new_cfg)
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

std::vector<ssb_frequency_t> ocudu::ocucp::generate_measurement_object_list(const cell_meas_manager_cfg& cfg,
                                                                            nr_cell_identity             serving_nci)
{
  ocudu_assert(cfg.cells.find(serving_nci) != cfg.cells.end(), "No cell config for nci={:#x}", serving_nci);

  // Add cells to lookup if report is configured
  std::vector<ssb_frequency_t> ssb_freqs;
  // Add serving cell if periodic report is configured
  const auto& serving_cell = cfg.cells.at(serving_nci);
  if (serving_cell.periodic_report_cfg_id.has_value() && is_complete(serving_cell.serving_cell_cfg)) {
    ssb_freqs.push_back(serving_cell.serving_cell_cfg.ssb_arfcn.value().value());
  }
  // Add neighbor cells if report is configured
  for (const auto& ncell : serving_cell.ncells) {
    ocudu_assert(cfg.cells.find(ncell.nci) != cfg.cells.end(), "No cell config for nci={:#x}", ncell.nci);
    const auto& cell_cfg = cfg.cells.at(ncell.nci);
    if (!ncell.report_cfg_ids.empty() && is_complete(cell_cfg.serving_cell_cfg)) {
      if (std::find(ssb_freqs.begin(), ssb_freqs.end(), cell_cfg.serving_cell_cfg.ssb_arfcn.value()) ==
          ssb_freqs.end()) {
        ssb_freqs.push_back(cell_cfg.serving_cell_cfg.ssb_arfcn.value().value());
      }
    }
  }

  return ssb_freqs;
}

std::vector<ssb_frequency_t> ocudu::ocucp::generate_cho_measurement_object_list(const cell_meas_manager_cfg& cfg,
                                                                                nr_cell_identity  serving_nci,
                                                                                span<const pci_t> candidate_pcis)
{
  ocudu_assert(cfg.cells.count(serving_nci), "No cell config for nci={:#x}", serving_nci);

  std::unordered_set<pci_t>           candidate_pcis_set(candidate_pcis.begin(), candidate_pcis.end());
  std::unordered_set<ssb_frequency_t> freqs;

  // Include the serving cell frequency. Required for inter-frequency handovers (e.g., A5 CHO).
  const auto& serving_cell_cfg = cfg.cells.at(serving_nci).serving_cell_cfg;
  if (is_complete(serving_cell_cfg)) {
    freqs.insert(serving_cell_cfg.ssb_arfcn->value());
  }

  for (const auto& ncell : cfg.cells.at(serving_nci).ncells) {
    auto it = cfg.cells.find(ncell.nci);
    if (it == cfg.cells.end()) {
      ocudulog::fetch_basic_logger(LOG_CHAN).warning("No cell config for nci={:#x}, skipping", ncell.nci);
      continue;
    }

    const auto& scfg = it->second.serving_cell_cfg;

    if (!is_complete(scfg) || !scfg.pci || (!candidate_pcis_set.empty() && !candidate_pcis_set.count(*scfg.pci))) {
      continue;
    }

    freqs.insert(scfg.ssb_arfcn->value());
  }

  return {freqs.begin(), freqs.end()};
}

void ocudu::ocucp::generate_report_config(const cell_meas_manager_cfg&  cfg,
                                          const nr_cell_identity        nci,
                                          const report_cfg_id_t         report_cfg_id,
                                          rrc_meas_cfg&                 meas_cfg,
                                          cell_meas_manager_ue_context& ue_meas_context)
{
  // add report cfg to add mod
  rrc_report_cfg_to_add_mod report_cfg_to_add_mod;
  report_cfg_to_add_mod.report_cfg_id = report_cfg_id;
  report_cfg_to_add_mod.report_cfg    = cfg.report_config_ids.at(report_cfg_id);
  meas_cfg.report_cfg_to_add_mod_list.push_back(report_cfg_to_add_mod);

  // Add meas id to link the cell and the report together.
  rrc_meas_id_to_add_mod meas_id_to_add_mod;
  meas_id_to_add_mod.meas_id       = ue_meas_context.allocate_meas_id();
  meas_id_to_add_mod.meas_obj_id   = ue_meas_context.nci_to_meas_obj_id.at(nci);
  meas_id_to_add_mod.report_cfg_id = report_cfg_id;
  meas_cfg.meas_id_to_add_mod_list.push_back(meas_id_to_add_mod);

  // add meas id to lookup
  const auto& serving_cell_cfg = cfg.cells.at(nci).serving_cell_cfg;
  ue_meas_context.meas_id_to_meas_context.emplace(meas_id_to_add_mod.meas_id,
                                                  meas_context_t{meas_id_to_add_mod.meas_obj_id,
                                                                 meas_id_to_add_mod.report_cfg_id,
                                                                 serving_cell_cfg.gnb_id_bit_length,
                                                                 serving_cell_cfg.nci,
                                                                 serving_cell_cfg.pci.value()});

  // Set T312 value in the measurement object if it's linked to (event-triggered) report config with T312 configured.
  const auto* event_triggered = std::get_if<rrc_event_trigger_cfg>(&report_cfg_to_add_mod.report_cfg);
  if (event_triggered != nullptr && event_triggered->t312.has_value()) {
    auto meas_obj_it = std::find_if(
        meas_cfg.meas_obj_to_add_mod_list.begin(),
        meas_cfg.meas_obj_to_add_mod_list.end(),
        [&](const rrc_meas_obj_to_add_mod& obj) { return obj.meas_obj_id == meas_id_to_add_mod.meas_obj_id; });
    if (meas_obj_it != meas_cfg.meas_obj_to_add_mod_list.end() && meas_obj_it->meas_obj_nr.has_value()) {
      meas_obj_it->meas_obj_nr.value().t312 = event_triggered->t312;
    }
  }
}

rrc_meas_obj_nr ocudu::ocucp::generate_measurement_object(const serving_cell_meas_config& cfg)
{
  rrc_meas_obj_nr meas_obj_nr;

  meas_obj_nr.ssb_freq = cfg.ssb_arfcn.has_value() ? std::optional{cfg.ssb_arfcn.value().value()} : std::nullopt;
  meas_obj_nr.ssb_subcarrier_spacing = cfg.ssb_scs;
  meas_obj_nr.smtc1                  = cfg.ssb_mtc;

  // Mandatory fields.
  meas_obj_nr.ref_sig_cfg.ssb_cfg_mob.emplace().derive_ssb_idx_from_cell = true;
  meas_obj_nr.nrof_ss_blocks_to_average.emplace()                        = 8; // TODO: remove hardcoded values
  meas_obj_nr.quant_cfg_idx                                              = 1; // TODO: remove hardcoded values
  meas_obj_nr.freq_band_ind_nr.emplace()                                 = nr_band_to_uint(cfg.band.value());

  // TODO: Add optional fields.

  return meas_obj_nr;
}

bool ocudu::ocucp::is_duplicate(const rrc_meas_obj_nr& obj_1, const rrc_meas_obj_nr& obj_2)
{
  // TS 38.331 section 5.5.2.1:
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

void ocudu::ocucp::log_meas_objects(const ocudulog::basic_logger&                               logger,
                                    const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& meas_objects)
{
  if (!meas_objects.empty()) {
    logger.debug("Measurement objects:");
    for (const auto& meas_obj : meas_objects) {
      logger.debug(" - ssb_freq={}: {}", meas_obj.first, meas_obj.second);
    }
  }
}

std::vector<report_cfg_id_t> ocudu::ocucp::collect_cond_trigger_report_configs(const cell_meas_manager_cfg& cfg,
                                                                               rrc_meas_cfg&                meas_cfg,
                                                                               const rrc_ue_capability_handler& ue_caps,
                                                                               ocudulog::basic_logger&          logger)
{
  if (!ue_caps.is_conditional_handover_supported()) {
    logger.debug("UE does not support CHO (Rel-16); no conditional trigger configs will be added");
    return {};
  }
  const bool     two_trigger_capable = ue_caps.is_conditional_handover_two_trigger_events_supported();
  const unsigned max_configs         = two_trigger_capable ? 2U : 1U;

  std::vector<report_cfg_id_t> cond_trigger_ids;
  for (const auto& [report_cfg_id, report_cfg] : cfg.report_config_ids) {
    if (cond_trigger_ids.size() >= max_configs) {
      break;
    }
    if (!std::holds_alternative<rrc_cond_trigger_cfg>(report_cfg)) {
      continue;
    }
    // Filter Rel-17 trigger types by UE capability.
    const auto& cond_trigger = std::get<rrc_cond_trigger_cfg>(report_cfg);
    const auto  event_id     = cond_trigger.cond_event_id.id;
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
    rrc_report_cfg_to_add_mod report_cfg_to_add;
    report_cfg_to_add.report_cfg_id = report_cfg_id;
    report_cfg_to_add.report_cfg    = report_cfg;
    meas_cfg.report_cfg_to_add_mod_list.push_back(report_cfg_to_add);
    cond_trigger_ids.push_back(report_cfg_id);
  }
  return cond_trigger_ids;
}

bool ocudu::ocucp::generate_cho_meas_ids(const cell_meas_manager_cfg&  cfg,
                                         span<const report_cfg_id_t>   cond_trigger_ids,
                                         rrc_meas_cfg&                 meas_cfg,
                                         cell_meas_manager_ue_context& ue_meas_context)
{
  for (const auto& mo : meas_cfg.meas_obj_to_add_mod_list) {
    for (const auto& report_cfg_id : cond_trigger_ids) {
      meas_id_t meas_id = ue_meas_context.allocate_meas_id();
      if (meas_id == meas_id_t::invalid) {
        return false;
      }

      meas_cfg.meas_id_to_add_mod_list.push_back({meas_id, mo.meas_obj_id, report_cfg_id});

      for (const auto& [context_nci, context_mo_id] : ue_meas_context.nci_to_meas_obj_id) {
        if (context_mo_id == mo.meas_obj_id) {
          const auto& cell_cfg = cfg.cells.at(context_nci);
          ocudu_assert(
              cell_cfg.serving_cell_cfg.pci.has_value(), "PCI must be set for CHO candidate nci={:#x}", context_nci);
          ue_meas_context.meas_id_to_meas_context.emplace(meas_id,
                                                          meas_context_t{mo.meas_obj_id,
                                                                         report_cfg_id,
                                                                         cell_cfg.serving_cell_cfg.gnb_id_bit_length,
                                                                         context_nci,
                                                                         cell_cfg.serving_cell_cfg.pci.value()});
          meas_cfg.nci_to_meas_ids[context_nci].push_back(meas_id);
        }
      }
    }
  }
  return true;
}
