// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_meas_manager_impl.h"
#include "cell_meas_manager_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cell_meas_manager_config.h"
#include "ocudu/ran/meas_types.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <set>
#include <utility>

using namespace ocudu;
using namespace ocucp;

/// Builds a config that only removes the measurement ids, objects and report configs of \c current, so a
/// UE whose measurements no longer apply can be told to drop them. Returns nullopt when there is nothing to
/// remove.
static std::optional<rrc_meas_cfg> make_removal_only_meas_config(const std::optional<rrc_meas_cfg>& current)
{
  if (!current.has_value()) {
    return std::nullopt;
  }
  rrc_meas_cfg rem_cfg;
  add_old_meas_config_to_rem_list(current.value(), rem_cfg);
  if (rem_cfg.meas_obj_to_rem_list.empty() && rem_cfg.meas_id_to_rem_list.empty() &&
      rem_cfg.report_cfg_to_rem_list.empty()) {
    return std::nullopt;
  }
  return rem_cfg;
}

cell_meas_manager::cell_meas_manager(const cell_meas_manager_config&       cfg_,
                                     const cell_meas_manager_dependencies& dependencies) :
  cfg(cfg_),
  mobility_mng_notifier(dependencies.mobility_mng_notifier),
  ue_mng(dependencies.ue_mng),
  logger(dependencies.logger)
{
  ocudu_assert(is_valid_configuration(cfg, ssb_freq_to_meas_object), "Invalid cell measurement configuration");
  generate_measurement_objects_for_serving_cells();
  log_cells(logger, cfg);
}

std::optional<rrc_meas_cfg>
cell_meas_manager::get_measurement_config(cu_cp_ue_index_t                   ue_index,
                                          nr_cell_identity                   serving_nci,
                                          const std::optional<rrc_meas_cfg>& current_meas_config,
                                          bool                               cond_meas,
                                          span<const pci_t>                  candidate_pcis)
{
  std::optional<rrc_meas_cfg> meas_cfg;

  // Find cell.
  if (cfg.cells.find(serving_nci) == cfg.cells.end()) {
    logger.debug("Couldn't find cell config for nci={:#x}", serving_nci);
    if (!cond_meas) {
      return remove_current_meas_config(ue_index, current_meas_config);
    }
    return meas_cfg;
  }
  const auto& cell_config = cfg.cells.at(serving_nci);

  // Measurement config is only generated if serving cell config is complete.
  if (!is_complete(cell_config.serving_cell_cfg)) {
    logger.debug("ue={}: Serving cell config is incomplete for nci={:#x}", ue_index, serving_nci);
    if (!cond_meas) {
      return remove_current_meas_config(ue_index, current_meas_config);
    }
    return meas_cfg;
  }

  // For regular measurement config, early-exit when no neighbors and no periodic report.
  if (!cond_meas and cell_config.ncells.empty() and !cell_config.periodic_report_cfg_id.has_value()) {
    logger.debug("ue={}: No neighbor cells configured and periodic serving cell reports disabled for nci={:#x}",
                 ue_index,
                 serving_nci);
    return remove_current_meas_config(ue_index, current_meas_config);
  }

  auto& ue_meas_context = ue_mng.get_measurement_context(ue_index);

  // Create fresh config.
  meas_cfg.emplace();
  auto& new_cfg = meas_cfg.value();

  // Request removal of old measurement config if it exists.
  if (current_meas_config.has_value()) {
    add_old_meas_config_to_rem_list(current_meas_config.value(), new_cfg);
  }

  // Clear existing measurement object ids and measurement ids in UE context.
  ue_meas_context.clear_meas_obj_ids();
  ue_meas_context.clear_meas_ids();

  // Build ssb_freqs vector.
  // For regular: serving cell (if periodic report configured) plus neighbors with report configs.
  // For CHO: candidate neighbors filtered by optional PCI set.
  std::vector<ssb_frequency_t> ssb_freqs;
  if (cond_meas) {
    ssb_freqs = generate_cho_measurement_object_list(cfg, serving_nci, candidate_pcis);
    if (ssb_freqs.empty()) {
      logger.warning("ue={}: No measurement objects match CHO candidate PCIs for nci={:#x}", ue_index, serving_nci);
      return std::nullopt;
    }
  } else {
    ssb_freqs = generate_measurement_object_list(cfg, serving_nci);
  }

  // TODO: Filter measurement objects using UE capabilities.

  // Add measurement object (MO).
  for (const auto& ssb_freq : ssb_freqs) {
    auto meas_obj_it  = ssb_freq_to_meas_object.find(ssb_freq);
    auto freq_ncis_it = ssb_freq_to_ncis.find(ssb_freq);
    if (meas_obj_it == ssb_freq_to_meas_object.end() or freq_ncis_it == ssb_freq_to_ncis.end()) {
      logger.warning("ue={}: No measurement object for ssb_freq={}, skipping", ue_index, ssb_freq);
      continue;
    }
    rrc_meas_obj_to_add_mod meas_obj_to_add;
    meas_obj_to_add.meas_obj_id = ue_meas_context.allocate_meas_obj_id();
    meas_obj_to_add.meas_obj_nr = meas_obj_it->second;

    // Add meas obj id to lookup.
    for (const auto& nci : freq_ncis_it->second) {
      if (cond_meas && nci == serving_nci) {
        // Skip the serving cell as is not a CHO candidate target.
        continue;
      }
      ue_meas_context.nci_to_meas_obj_id.emplace(nci, meas_obj_to_add.meas_obj_id);

      // Find this NCI in the serving cell's neighbour list to retrieve NTN info.
      const auto ncell_it = std::find_if(cell_config.ncells.begin(),
                                         cell_config.ncells.end(),
                                         [nci](const neighbor_cell_meas_config& nc) { return nc.nci == nci; });

      auto nci_cfg_it = cfg.cells.find(nci);
      if (nci_cfg_it == cfg.cells.end()) {
        logger.warning("ue={}: No cell config for nci={:#x}, skipping", ue_index, nci.value());
        continue;
      }
      if (cond_meas) {
        const auto&          ncell_cfg = nci_cfg_it->second;
        rrc_cells_to_add_mod cell_to_add;
        cell_to_add.pci = ncell_cfg.serving_cell_cfg.pci.value();
        if (ncell_it != cell_config.ncells.end()) {
          cell_to_add.ntn_neighbour_info = ncell_it->ntn_neighbour_info;
          cell_to_add.ntn_polarization   = ncell_it->ntn_polarization;
        }
        meas_obj_to_add.meas_obj_nr->cells_to_add_mod_list.push_back(cell_to_add);
      } else if (ncell_it != cell_config.ncells.end() &&
                 (ncell_it->ntn_neighbour_info.has_value() || ncell_it->ntn_polarization.has_value())) {
        // For non-CHO: add cell to cells_to_add_mod_list only when NTN info is present (needed for
        // cells_to_add_mod_list_ext_v1800/v1710 in MeasObjectNR).
        const auto& ncell_cfg = nci_cfg_it->second;
        if (ncell_cfg.serving_cell_cfg.pci.has_value()) {
          rrc_cells_to_add_mod cell_to_add;
          cell_to_add.pci                = ncell_cfg.serving_cell_cfg.pci.value();
          cell_to_add.ntn_neighbour_info = ncell_it->ntn_neighbour_info;
          cell_to_add.ntn_polarization   = ncell_it->ntn_polarization;
          meas_obj_to_add.meas_obj_nr->cells_to_add_mod_list.push_back(cell_to_add);
        }
      }
    }
    new_cfg.meas_obj_to_add_mod_list.push_back(std::move(meas_obj_to_add));
  }

  // Report config + meas IDs.
  if (!cond_meas) {
    for (const auto& ssb_freq : ssb_freqs) {
      if (cell_config.serving_cell_cfg.ssb_arfcn.value() == ssb_freq &&
          cell_config.periodic_report_cfg_id.has_value()) {
        logger.debug("ue={}: Adding periodic report config for nci={:#x}", ue_index, serving_nci);
        generate_report_config(cfg, serving_nci, cell_config.periodic_report_cfg_id.value(), new_cfg, ue_meas_context);
      }

      for (const auto& ncell : cell_config.ncells) {
        auto ncell_it = cfg.cells.find(ncell.nci);
        if (ncell_it == cfg.cells.end()) {
          logger.warning("ue={}: No cell config for neighbor nci={:#x}, skipping", ue_index, ncell.nci.value());
          continue;
        }
        if (is_complete(ncell_it->second.serving_cell_cfg) &&
            ncell_it->second.serving_cell_cfg.ssb_arfcn.value() == ssb_freq) {
          logger.debug("ue={}: Adding neighbor cell nci={:#x} to measurement config", ue_index, ncell.nci);
          for (const auto& report_cfg_id : ncell.report_cfg_ids) {
            // Skip conditional triggers.
            if (is_cond_trigger_report_config(cfg, report_cfg_id)) {
              continue;
            }
            generate_report_config(cfg, ncell.nci, report_cfg_id, new_cfg, ue_meas_context);
          }
        }
      }
    }

    // Ensure reports are not repeated.
    auto cmp_id = [](const rrc_report_cfg_to_add_mod& lhs, const rrc_report_cfg_to_add_mod& rhs) {
      return lhs.report_cfg_id < rhs.report_cfg_id;
    };
    std::sort(new_cfg.report_cfg_to_add_mod_list.begin(), new_cfg.report_cfg_to_add_mod_list.end(), cmp_id);
    auto eq_id = [](const rrc_report_cfg_to_add_mod& lhs, const rrc_report_cfg_to_add_mod& rhs) {
      return lhs.report_cfg_id == rhs.report_cfg_id;
    };
    auto end_it =
        std::unique(new_cfg.report_cfg_to_add_mod_list.begin(), new_cfg.report_cfg_to_add_mod_list.end(), eq_id);
    new_cfg.report_cfg_to_add_mod_list.erase(end_it, new_cfg.report_cfg_to_add_mod_list.end());
    std::sort(new_cfg.report_cfg_to_add_mod_list.begin(), new_cfg.report_cfg_to_add_mod_list.end(), cmp_id);
  } else {
    const cu_cp_ue* ue = ue_mng.find_ue(ue_index);
    if (ue == nullptr || ue->get_rrc_ue() == nullptr) {
      logger.error("ue={}: CHO meas config requested but UE/RRC context is missing", ue_index);
      return std::nullopt;
    }

    const auto cond_trigger_ids = collect_cond_trigger_report_configs(cfg, new_cfg, *ue->get_rrc_ue(), logger);
    if (cond_trigger_ids.empty()) {
      logger.warning("ue={}: No conditional trigger report configs found for CHO measurement config", ue_index);
      return std::nullopt;
    }

    if (!generate_cho_meas_ids(cfg, cond_trigger_ids, new_cfg, ue_meas_context)) {
      logger.error("ue={}: Failed to allocate measurement ID for CHO measurement config", ue_index);
      return std::nullopt;
    }

    logger.info("ue={}: Generated CHO measurement config with {} filtered MOs, {} conditional triggers, "
                "{} meas IDs, and {} NCI mappings",
                ue_index,
                new_cfg.meas_obj_to_add_mod_list.size(),
                cond_trigger_ids.size(),
                new_cfg.meas_id_to_add_mod_list.size(),
                new_cfg.nci_to_meas_ids.size());
  }

  // Add quantity config.
  rrc_quant_cfg    quant_cfg;
  rrc_quant_cfg_nr quant_cfg_nr;
  quant_cfg_nr.quant_cfg_cell.ssb_filt_cfg.filt_coef_rsrp    = 6; // TODO: remove hardcoded values
  quant_cfg_nr.quant_cfg_cell.csi_rs_filt_cfg.filt_coef_rsrp = 6; // TODO: remove hardcoded values
  quant_cfg.quant_cfg_nr_list.push_back(quant_cfg_nr);

  new_cfg.quant_cfg = quant_cfg;

  // Keep in the removal lists only what the new config does not set up again.
  if (current_meas_config.has_value()) {
    prune_redundant_rem_list_entries(current_meas_config.value(), new_cfg);
  }

  return meas_cfg;
}

std::optional<rrc_meas_cfg>
cell_meas_manager::remove_current_meas_config(cu_cp_ue_index_t                   ue_index,
                                              const std::optional<rrc_meas_cfg>& current_meas_config)
{
  std::optional<rrc_meas_cfg> rem_cfg = make_removal_only_meas_config(current_meas_config);
  if (rem_cfg.has_value()) {
    // The UE is left without measurements: drop its measurement id bookkeeping.
    auto& ue_meas_context = ue_mng.get_measurement_context(ue_index);
    ue_meas_context.clear_meas_obj_ids();
    ue_meas_context.clear_meas_ids();
  }
  return rem_cfg;
}

std::optional<cell_meas_config> cell_meas_manager::get_cell_config(nr_cell_identity nci)
{
  std::optional<cell_meas_config> cell_cfg;
  if (cfg.cells.find(nci) != cfg.cells.end()) {
    cell_cfg = cfg.cells.at(nci);
  }
  return cell_cfg;
}

std::vector<pci_t> cell_meas_manager::get_neighbor_pcis(nr_cell_identity serving_nci) const
{
  auto serving_it = cfg.cells.find(serving_nci);
  if (serving_it == cfg.cells.end()) {
    return {};
  }

  std::set<pci_t> neighbor_pcis;
  for (const auto& ncell : serving_it->second.ncells) {
    auto neighbor_it = cfg.cells.find(ncell.nci);
    if (neighbor_it == cfg.cells.end()) {
      continue;
    }
    if (!neighbor_it->second.serving_cell_cfg.pci.has_value()) {
      continue;
    }
    neighbor_pcis.insert(neighbor_it->second.serving_cell_cfg.pci.value());
  }

  return {neighbor_pcis.begin(), neighbor_pcis.end()};
}

bool cell_meas_manager::update_cell_config(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg)
{
  // Store old config to revert if new config is invalid.
  cell_meas_manager_config tmp_cfg = cfg;

  if (cfg.cells.find(nci) == cfg.cells.end()) {
    logger.debug("No configuration to update for nci={:#x}. Adding configuration", nci);

    cell_meas_config meas_cfg;
    meas_cfg.serving_cell_cfg = serv_cell_cfg;

    cfg.cells.emplace(nci, meas_cfg);
  } else {
    logger.debug("Updating measurement configuration for nci={:#x}", nci);

    // Update serving cell config.
    cfg.cells.at(nci).serving_cell_cfg = serv_cell_cfg;
  }

  if (!is_valid_configuration(cfg, ssb_freq_to_meas_object)) {
    logger.warning("Invalid cell measurement configuration");
    cfg = tmp_cfg;
    return false;
  }

  if (!is_complete(serv_cell_cfg)) {
    // The parameters are replaced as a whole: a cell that stops being complete must not stay attached to its
    // measurement object with the parameters it had before.
    remove_measurement_object(nci);
    logger.debug("Added/Updated incomplete cell measurement configuration for nci={:#x}", nci);
  } else {
    // Only update measurement object if the configuration is complete.
    update_measurement_object(nci, serv_cell_cfg);
    log_meas_objects(logger, ssb_freq_to_meas_object);
  }

  log_cells(logger, cfg);

  return true;
}

bool cell_meas_manager::remove_cell_config(nr_cell_identity nci)
{
  auto cell_it = cfg.cells.find(nci);
  if (cell_it == cfg.cells.end()) {
    logger.warning("Cannot remove cell nci={:#x}. Cause: No cell config for this NCI", nci.value());
    return false;
  }

  // Detach the cell from the measurement object lookups.
  remove_measurement_object(nci);

  cfg.cells.erase(cell_it);

  // Cascade: drop the neighbor relations of other cells pointing at the removed cell.
  for (auto& [other_nci, other_cell] : cfg.cells) {
    auto removed_from = std::remove_if(other_cell.ncells.begin(),
                                       other_cell.ncells.end(),
                                       [nci](const neighbor_cell_meas_config& ncell) { return ncell.nci == nci; });
    if (removed_from != other_cell.ncells.end()) {
      other_cell.ncells.erase(removed_from, other_cell.ncells.end());
      logger.info("Removed neighbor relation from nci={:#x} to removed cell nci={:#x}", other_nci.value(), nci.value());
    }
  }

  logger.info("Removed cell nci={:#x}", nci.value());
  log_cells(logger, cfg);

  return true;
}

bool cell_meas_manager::add_or_update_neighbor(nr_cell_identity             serving_nci,
                                               nr_cell_identity             neighbor_nci,
                                               std::vector<report_cfg_id_t> report_cfg_ids)
{
  if (serving_nci == neighbor_nci) {
    logger.warning("Cannot add neighbor relation for nci={:#x}. Cause: A cell cannot neighbor itself",
                   serving_nci.value());
    return false;
  }
  auto serving_it = cfg.cells.find(serving_nci);
  if (serving_it == cfg.cells.end()) {
    logger.warning("Cannot add neighbor relation. Cause: No cell config for serving nci={:#x}", serving_nci.value());
    return false;
  }
  if (cfg.cells.find(neighbor_nci) == cfg.cells.end()) {
    logger.warning("Cannot add neighbor relation. Cause: No cell config for neighbor nci={:#x}", neighbor_nci.value());
    return false;
  }
  for (report_cfg_id_t report_cfg_id : report_cfg_ids) {
    auto report_cfg_it = cfg.report_config_ids.find(report_cfg_id);
    if (report_cfg_it == cfg.report_config_ids.end()) {
      logger.warning("Cannot add neighbor relation. Cause: No report config with id={}", to_underlying(report_cfg_id));
      return false;
    }
    if (std::holds_alternative<rrc_periodical_report_cfg>(report_cfg_it->second)) {
      logger.warning("Cannot add neighbor relation. Cause: Report config id={} is periodical (serving cell only)",
                     to_underlying(report_cfg_id));
      return false;
    }
  }

  auto ncell_it = std::find_if(serving_it->second.ncells.begin(),
                               serving_it->second.ncells.end(),
                               [neighbor_nci](const neighbor_cell_meas_config& nc) { return nc.nci == neighbor_nci; });
  if (ncell_it != serving_it->second.ncells.end()) {
    ncell_it->report_cfg_ids = std::move(report_cfg_ids);
    logger.info("Updated neighbor relation from nci={:#x} to nci={:#x}", serving_nci.value(), neighbor_nci.value());
  } else {
    neighbor_cell_meas_config& ncell = serving_it->second.ncells.emplace_back();
    ncell.nci                        = neighbor_nci;
    ncell.report_cfg_ids             = std::move(report_cfg_ids);
    logger.info("Added neighbor relation from nci={:#x} to nci={:#x}", serving_nci.value(), neighbor_nci.value());
  }
  log_cells(logger, cfg);

  return true;
}

bool cell_meas_manager::remove_neighbor(nr_cell_identity serving_nci, nr_cell_identity neighbor_nci)
{
  auto serving_it = cfg.cells.find(serving_nci);
  if (serving_it == cfg.cells.end()) {
    logger.warning("Cannot remove neighbor relation. Cause: No cell config for serving nci={:#x}", serving_nci.value());
    return false;
  }
  auto ncell_it = std::find_if(serving_it->second.ncells.begin(),
                               serving_it->second.ncells.end(),
                               [neighbor_nci](const neighbor_cell_meas_config& nc) { return nc.nci == neighbor_nci; });
  if (ncell_it == serving_it->second.ncells.end()) {
    logger.warning("Cannot remove neighbor relation. Cause: nci={:#x} is not a neighbor of nci={:#x}",
                   neighbor_nci.value(),
                   serving_nci.value());
    return false;
  }
  serving_it->second.ncells.erase(ncell_it);
  logger.info("Removed neighbor relation from nci={:#x} to nci={:#x}", serving_nci.value(), neighbor_nci.value());
  log_cells(logger, cfg);

  return true;
}

bool cell_meas_manager::add_or_update_report_config(report_cfg_id_t report_cfg_id, const rrc_report_cfg_nr& report_cfg)
{
  const bool is_periodical = std::holds_alternative<rrc_periodical_report_cfg>(report_cfg);

  // A referenced report config must keep the report-type class its references rely on.
  for (const auto& [nci, cell] : cfg.cells) {
    if (is_periodical) {
      for (const auto& ncell : cell.ncells) {
        if (std::find(ncell.report_cfg_ids.begin(), ncell.report_cfg_ids.end(), report_cfg_id) !=
            ncell.report_cfg_ids.end()) {
          logger.warning("Cannot update report config id={} to periodical. Cause: Referenced by a neighbor "
                         "relation of nci={:#x}",
                         to_underlying(report_cfg_id),
                         nci.value());
          return false;
        }
      }
    } else if (cell.periodic_report_cfg_id == report_cfg_id) {
      logger.warning("Cannot update report config id={} to non-periodical. Cause: Used as periodic report of nci={:#x}",
                     to_underlying(report_cfg_id),
                     nci.value());
      return false;
    }
  }

  const bool existed                   = cfg.report_config_ids.count(report_cfg_id) > 0;
  cfg.report_config_ids[report_cfg_id] = report_cfg;
  logger.info("{} report config id={}", existed ? "Updated" : "Added", to_underlying(report_cfg_id));

  return true;
}

bool cell_meas_manager::remove_report_config(report_cfg_id_t report_cfg_id)
{
  auto report_cfg_it = cfg.report_config_ids.find(report_cfg_id);
  if (report_cfg_it == cfg.report_config_ids.end()) {
    logger.warning("Cannot remove report config id={}. Cause: No report config with this id",
                   to_underlying(report_cfg_id));
    return false;
  }
  for (const auto& [nci, cell] : cfg.cells) {
    if (cell.periodic_report_cfg_id == report_cfg_id) {
      logger.warning("Cannot remove report config id={}. Cause: Used as periodic report of nci={:#x}",
                     to_underlying(report_cfg_id),
                     nci.value());
      return false;
    }
    for (const auto& ncell : cell.ncells) {
      if (std::find(ncell.report_cfg_ids.begin(), ncell.report_cfg_ids.end(), report_cfg_id) !=
          ncell.report_cfg_ids.end()) {
        logger.warning("Cannot remove report config id={}. Cause: Referenced by a neighbor relation of nci={:#x}",
                       to_underlying(report_cfg_id),
                       nci.value());
        return false;
      }
    }
  }
  cfg.report_config_ids.erase(report_cfg_it);
  logger.info("Removed report config id={}", to_underlying(report_cfg_id));

  return true;
}

bool cell_meas_manager::set_periodic_report_config(nr_cell_identity nci, std::optional<report_cfg_id_t> report_cfg_id)
{
  auto cell_it = cfg.cells.find(nci);
  if (cell_it == cfg.cells.end()) {
    logger.warning("Cannot set periodic report. Cause: No cell config for nci={:#x}", nci.value());
    return false;
  }
  if (report_cfg_id.has_value()) {
    auto report_cfg_it = cfg.report_config_ids.find(report_cfg_id.value());
    if (report_cfg_it == cfg.report_config_ids.end()) {
      logger.warning("Cannot set periodic report for nci={:#x}. Cause: No report config with id={}",
                     nci.value(),
                     to_underlying(report_cfg_id.value()));
      return false;
    }
    if (!std::holds_alternative<rrc_periodical_report_cfg>(report_cfg_it->second)) {
      logger.warning("Cannot set periodic report for nci={:#x}. Cause: Report config id={} is not periodical",
                     nci.value(),
                     to_underlying(report_cfg_id.value()));
      return false;
    }
  }
  cell_it->second.periodic_report_cfg_id = report_cfg_id;
  logger.info("Set periodic report of nci={:#x} to id={}",
              nci.value(),
              report_cfg_id.has_value() ? std::to_string(to_underlying(report_cfg_id.value())) : "none");

  return true;
}

bool cell_meas_manager::update_ntn_neighbour_info(nr_cell_identity                             serving_nci,
                                                  span<const rrc_ntn_neighbour_cell_info_item> ncells)
{
  auto cell_it = cfg.cells.find(serving_nci);
  if (cell_it == cfg.cells.end()) {
    logger.warning("Received NTN neighbour info update for unknown serving cell nci={:#x}", serving_nci);
    return false;
  }

  bool updated = false;
  for (const rrc_ntn_neighbour_cell_info_item& item : ncells) {
    auto ncell_it = std::find_if(cell_it->second.ncells.begin(),
                                 cell_it->second.ncells.end(),
                                 [&item](const neighbor_cell_meas_config& nc) { return nc.nci == item.nci; });
    if (ncell_it == cell_it->second.ncells.end()) {
      logger.warning("Received NTN neighbour info update for unknown neighbour nci={:#x} of serving cell nci={:#x}",
                     item.nci,
                     serving_nci);
      continue;
    }
    ncell_it->ntn_neighbour_info = item.info;
    ncell_it->ntn_polarization   = item.polarization;
    updated                      = true;
  }

  if (updated) {
    logger.debug("Updated NTN neighbour info for serving cell nci={:#x}", serving_nci);
  }
  return updated;
}

static std::optional<uint8_t> get_ssb_rsrp(const rrc_meas_result_nr& meas_result)
{
  std::optional<uint8_t> rsrp;
  if (meas_result.cell_results.results_ssb_cell.has_value()) {
    if (meas_result.cell_results.results_ssb_cell.value().rsrp.has_value()) {
      rsrp = meas_result.cell_results.results_ssb_cell.value().rsrp.value();
    }
  }
  return rsrp;
}

static std::optional<pci_t> find_strongest_neighbor(cu_cp_ue_index_t        ue_index,
                                                    const rrc_meas_results& meas_results,
                                                    ocudulog::basic_logger& logger,
                                                    std::optional<uint8_t>  periodic_ho_rsrp_offset = std::nullopt)
{
  std::optional<pci_t> strongest_neighbor;
  // Find strongest neighbor cell.
  if (meas_results.meas_result_neigh_cells.has_value()) {
    // Find strongest neighbor here.
    std::optional<uint8_t> serv_cell_rsrp;

    // Extract RSRP of SSB for servCellId 0.
    if (meas_results.meas_result_serving_mo_list.contains(0)) {
      serv_cell_rsrp = get_ssb_rsrp(meas_results.meas_result_serving_mo_list[0].meas_result_serving_cell);
    }

    if (serv_cell_rsrp.has_value()) {
      uint8_t max_rsrp = serv_cell_rsrp.value();

      for (const auto& report : meas_results.meas_result_neigh_cells.value().meas_result_list_nr) {
        std::optional<uint8_t> neighbor_rsrp = get_ssb_rsrp(report);
        if (neighbor_rsrp.has_value()) {
          if (neighbor_rsrp.value() > max_rsrp + periodic_ho_rsrp_offset.value_or(0)) {
            // Found stronger neighbor, take note of it's details.
            max_rsrp           = neighbor_rsrp.value();
            strongest_neighbor = report.pci;
          }
        }
      }

      if (strongest_neighbor.has_value()) {
        logger.info("ue={}: Neighbor PCI={} (ssb_rsrp={}) stronger than current serving cell (ssb_rsrp={})",
                    ue_index,
                    strongest_neighbor.value(),
                    max_rsrp,
                    serv_cell_rsrp.value());
      }
    }
  }
  return strongest_neighbor;
}

void cell_meas_manager::report_measurement(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  logger.debug("ue={}: Received measurement result with meas_id={}", ue_index, fmt::underlying(meas_results.meas_id));

  auto& ue_meas_context = ue_mng.get_measurement_context(ue_index);

  // Verify meas_id is valid.
  if (ue_meas_context.meas_id_to_meas_context.find(meas_results.meas_id) ==
      ue_meas_context.meas_id_to_meas_context.end()) {
    logger.debug(
        "ue={}: Measurement result for unknown meas_id={} received", ue_index, fmt::underlying(meas_results.meas_id));
    return;
  }

  // Store measurement results.
  store_measurement_results(ue_index, meas_results);

  auto& meas_ctxt = ue_meas_context.meas_id_to_meas_context.at(meas_results.meas_id);

  // The measurement context references the cell configuration by NCI: look it up guarded, so a measurement
  // report racing a configuration change cannot dereference an erased entry.
  auto serving_cell_it = cfg.cells.find(meas_ctxt.nci);
  if (serving_cell_it == cfg.cells.end()) {
    logger.warning("ue={}: Ignoring measurement result for meas_id={}. Cause: No cell config for nci={:#x}",
                   ue_index,
                   fmt::underlying(meas_results.meas_id),
                   meas_ctxt.nci.value());
    return;
  }
  const cell_meas_config& serving_cell = serving_cell_it->second;

  // Handle periodic measurement results.
  if (serving_cell.periodic_report_cfg_id.has_value() &&
      serving_cell.periodic_report_cfg_id.value() == meas_ctxt.report_cfg_id) {
    uint8_t periodic_ho_rsrp_offset = 0;
    auto    report_cfg_it           = cfg.report_config_ids.find(serving_cell.periodic_report_cfg_id.value());
    if (report_cfg_it == cfg.report_config_ids.end()) {
      logger.warning("ue={}: Ignoring measurement result for meas_id={}. Cause: No report config with id={}",
                     ue_index,
                     fmt::underlying(meas_results.meas_id),
                     to_underlying(serving_cell.periodic_report_cfg_id.value()));
      return;
    }
    if (const auto* periodical = std::get_if<rrc_periodical_report_cfg>(&report_cfg_it->second);
        periodical != nullptr) {
      if (periodical->periodic_ho_rsrp_offset == -1) {
        logger.debug("ue={}: Handover from periodic measurements is disabled", ue_index);
        return;
      }
      periodic_ho_rsrp_offset = uint8_t(periodical->periodic_ho_rsrp_offset);
    }

    // Find strongest neighbor cell.
    std::optional<pci_t> strongest_neighbor =
        find_strongest_neighbor(ue_index, meas_results, logger, periodic_ho_rsrp_offset);
    if (strongest_neighbor.has_value()) {
      for (const auto& ncell : serving_cell.ncells) {
        auto ncell_it = cfg.cells.find(ncell.nci);
        if (ncell_it == cfg.cells.end()) {
          logger.warning("ue={}: No cell config for neighbor nci={:#x}, skipping", ue_index, ncell.nci.value());
          continue;
        }
        const cell_meas_config& ncell_cfg = ncell_it->second;
        if (ncell_cfg.serving_cell_cfg.pci.has_value() &&
            ncell_cfg.serving_cell_cfg.pci.value() == strongest_neighbor.value()) {
          // Report cell.
          mobility_mng_notifier.on_neighbor_better_than_spcell(
              ue_index,
              ncell_cfg.serving_cell_cfg.nci.gnb_id(ncell_cfg.serving_cell_cfg.gnb_id_bit_length),
              ncell_cfg.serving_cell_cfg.nci,
              strongest_neighbor.value(),
              ncell_cfg.serving_cell_cfg.plmn,
              ncell_cfg.serving_cell_cfg.tac);
          return;
        }
      }
    }
  } else {
    // For meas_id with e.g. A3 event configured:

    // Iterate through serving cell results and check if best neighbor is reported.
    for (const auto& serv_cell : meas_results.meas_result_serving_mo_list) {
      if (serv_cell.meas_result_best_neigh_cell.has_value()) {
        // Report this cell.
        if (serv_cell.meas_result_best_neigh_cell.value().pci.has_value()) {
          mobility_mng_notifier.on_neighbor_better_than_spcell(
              ue_index,
              meas_ctxt.nci.gnb_id(meas_ctxt.gnb_id_bit_length),
              meas_ctxt.nci,
              serv_cell.meas_result_best_neigh_cell.value().pci.value(),
              serving_cell.serving_cell_cfg.plmn,
              serving_cell.serving_cell_cfg.tac);
          return;
        }
      }
    }

    // Find strongest neighbor cell.
    std::optional<pci_t> strongest_neighbor = find_strongest_neighbor(ue_index, meas_results, logger);
    if (strongest_neighbor.has_value()) {
      // Report cell.
      mobility_mng_notifier.on_neighbor_better_than_spcell(ue_index,
                                                           meas_ctxt.nci.gnb_id(meas_ctxt.gnb_id_bit_length),
                                                           meas_ctxt.nci,
                                                           strongest_neighbor.value(),
                                                           serving_cell.serving_cell_cfg.plmn,
                                                           serving_cell.serving_cell_cfg.tac);
      return;
    }
  }
}

void cell_meas_manager::generate_measurement_objects_for_serving_cells()
{
  for (const auto& cell : cfg.cells) {
    if (is_complete(cell.second.serving_cell_cfg)) {
      update_measurement_object(cell.first, cell.second.serving_cell_cfg);
    }
  }

  log_meas_objects(logger, ssb_freq_to_meas_object);
}

void cell_meas_manager::update_measurement_object(nr_cell_identity                nci,
                                                  const serving_cell_meas_config& serving_cell_cfg)
{
  ocudu_assert(is_complete(serving_cell_cfg), "Incomplete measurement object update for nci={:#x}", nci);

  // Detach the cell from the frequency of any previous configuration first, so a repeated update (e.g. a DU
  // re-attach) does not append a duplicate lookup entry and a frequency change does not leave a stale one.
  remove_measurement_object(nci);

  ssb_frequency_t ssb_freq = serving_cell_cfg.ssb_arfcn.value().value();

  // Add to lookup.
  auto& freq_ncis = ssb_freq_to_ncis[ssb_freq];
  if (std::find(freq_ncis.begin(), freq_ncis.end(), nci) == freq_ncis.end()) {
    freq_ncis.push_back(nci);
  }
  nci_to_serving_cell_meas_config[serving_cell_cfg.nci] = serving_cell_cfg;

  if (ssb_freq_to_meas_object.find(ssb_freq) != ssb_freq_to_meas_object.end()) {
    // If the measurement object is already present, we ignore the duplicate.
    logger.debug("Measurement object for ssb_freq={} already exists", ssb_freq);
    return;
  }
  ssb_freq_to_meas_object.emplace(ssb_freq, generate_measurement_object(serving_cell_cfg));
}

void cell_meas_manager::remove_measurement_object(nr_cell_identity nci)
{
  // The stored serving-cell config remembers the frequency the cell was last attached to.
  auto old_cfg_it = nci_to_serving_cell_meas_config.find(nci);
  if (old_cfg_it == nci_to_serving_cell_meas_config.end()) {
    return;
  }
  ssb_frequency_t old_ssb_freq = old_cfg_it->second.ssb_arfcn.value().value();

  auto freq_ncis_it = ssb_freq_to_ncis.find(old_ssb_freq);
  if (freq_ncis_it != ssb_freq_to_ncis.end()) {
    auto& freq_ncis = freq_ncis_it->second;
    freq_ncis.erase(std::remove(freq_ncis.begin(), freq_ncis.end(), nci), freq_ncis.end());
    if (freq_ncis.empty()) {
      // Last cell on this frequency: drop the measurement object with it.
      ssb_freq_to_ncis.erase(freq_ncis_it);
      ssb_freq_to_meas_object.erase(old_ssb_freq);
    }
  }

  nci_to_serving_cell_meas_config.erase(old_cfg_it);
}

expected<std::pair<unsigned, nr_cell_identity>> cell_meas_manager::find_neighbour_nci(pci_t pci)
{
  for (const auto& [nci, cell_cfg] : cfg.cells) {
    if (cell_cfg.serving_cell_cfg.pci == pci) {
      return std::make_pair(cell_cfg.serving_cell_cfg.gnb_id_bit_length, nci);
    }
  }
  return make_unexpected(default_error_t{});
}

static expected<nr_cell_identity, std::string> find_nci(const cell_meas_manager_config& meas_mng_cfg, pci_t pci)
{
  for (const auto& [nci, cell_cfg] : meas_mng_cfg.cells) {
    if (cell_cfg.serving_cell_cfg.pci == pci) {
      return nci;
    }
  }
  return make_unexpected(fmt::format("No NCI found for PCI={}", pci));
}

static expected<cell_measurement_positioning_info, std::string> generate_measurement_objects_for_positioning(
    cu_cp_ue_index_t                                            ue_index,
    ue_manager&                                                 ue_mng,
    const cell_meas_manager_config&                             cfg,
    const std::map<nr_cell_identity, serving_cell_meas_config>& nci_to_serving_cell_meas_config,
    const rrc_meas_results&                                     meas_results,
    ocudulog::basic_logger&                                     logger)
{
  cu_cp_ue* ue = ue_mng.find_ue(ue_index);
  if (ue == nullptr) {
    return make_unexpected(fmt::format("UE not found", ue_index));
  }

  cell_meas_manager_ue_context& ue_meas_context = ue_mng.get_measurement_context(ue_index);
  ocudu_assert(ue_meas_context.meas_id_to_meas_context.find(meas_results.meas_id) !=
                   ue_meas_context.meas_id_to_meas_context.end(),
               "ue={}: Measurement result for unknown meas_id={} received",
               ue_index,
               fmt::underlying(meas_results.meas_id));

  meas_context_t& meas_ctxt = ue_meas_context.meas_id_to_meas_context.at(meas_results.meas_id);

  // Prepare measurement to forward measurement to CU-CP.
  cell_measurement_positioning_info pos_info;
  // Prepare serving cell measurement.
  {
    pci_t                    serving_cell_pci = ue->get_pci();
    nr_cell_identity         serving_nci;
    serving_cell_meas_config serv_cell_cfg;
    if (meas_ctxt.pci == serving_cell_pci) {
      // We received a measurement for the serving cell.
      serving_nci = meas_ctxt.nci;

    } else {
      // Find the serving cell.
      auto nci = find_nci(cfg, serving_cell_pci);
      if (!nci.has_value()) {
        return make_unexpected(fmt::format("{}", nci.error()));
      }
      serving_nci = nci.value();
    }

    if (nci_to_serving_cell_meas_config.find(meas_ctxt.nci) == nci_to_serving_cell_meas_config.end()) {
      return make_unexpected(fmt::format("No serving cell config found for nci={:#x}", meas_ctxt.nci));
    }
    serv_cell_cfg            = nci_to_serving_cell_meas_config.at(meas_ctxt.nci);
    pos_info.serving_cell_id = nr_cell_global_id_t{serv_cell_cfg.plmn, serv_cell_cfg.nci};
    pos_info.cell_measurements.emplace(serv_cell_cfg.nci,
                                       cell_measurement_positioning_info::cell_measurement_item_t{
                                           nr_cell_global_id_t{serv_cell_cfg.plmn, serv_cell_cfg.nci},
                                           serv_cell_cfg.ssb_arfcn.value(),
                                           meas_results.meas_result_serving_mo_list.begin()->meas_result_serving_cell});

    // TODO: Configure rs_idx_results and replace hardcoded values.
    rrc_meas_result_nr& meas_res = pos_info.cell_measurements.at(serv_cell_cfg.nci).meas_result;

    rrc_meas_result_nr::rs_idx_results_ rs_idx_results;
    if (meas_res.cell_results.results_ssb_cell.has_value()) {
      rs_idx_results.results_ssb_idxes.emplace(1, rrc_results_per_ssb_idx{1, meas_res.cell_results.results_ssb_cell});
    }
    if (meas_res.cell_results.results_csi_rs_cell.has_value()) {
      rs_idx_results.results_csi_rs_idxes.emplace(
          1, rrc_results_per_csi_rs_idx{1, meas_res.cell_results.results_csi_rs_cell});
    }
    meas_res.rs_idx_results = rs_idx_results;

    // TODO: Handle multiple serving cells.
  }
  // Prepare neighbor cell measurement.
  {
    if (meas_results.meas_result_neigh_cells.has_value()) {
      for (const rrc_meas_result_nr& neigh_meas_result : meas_results.meas_result_neigh_cells->meas_result_list_nr) {
        if (!neigh_meas_result.pci.has_value()) {
          logger.debug("ue={}: Neighbor cell measurement without PCI received", ue_index);
          continue;
        }

        // Find the serving cell config of the neighbor cell.
        expected<nr_cell_identity, std::string> nci = find_nci(cfg, neigh_meas_result.pci.value());
        if (!nci.has_value()) {
          return make_unexpected(fmt::format("{}", nci.error()));
        }

        if (nci_to_serving_cell_meas_config.find(nci.value()) == nci_to_serving_cell_meas_config.end()) {
          return make_unexpected(fmt::format("No serving cell config found for nci={:#x}", nci.value()));
        }
        serving_cell_meas_config ncell_meas_cfg = nci_to_serving_cell_meas_config.at(nci.value());
        pos_info.cell_measurements.emplace(nci.value(),
                                           cell_measurement_positioning_info::cell_measurement_item_t{
                                               nr_cell_global_id_t{ncell_meas_cfg.plmn, nci.value()},
                                               ncell_meas_cfg.ssb_arfcn.value(),
                                               neigh_meas_result});

        // TODO: Configure rs_idx_results and replace hardcoded values.
        rrc_meas_result_nr& meas_res = pos_info.cell_measurements.at(nci.value()).meas_result;

        rrc_meas_result_nr::rs_idx_results_ rs_idx_results;
        if (meas_res.cell_results.results_ssb_cell.has_value()) {
          rs_idx_results.results_ssb_idxes.emplace(1,
                                                   rrc_results_per_ssb_idx{1, meas_res.cell_results.results_ssb_cell});
        }
        if (meas_res.cell_results.results_csi_rs_cell.has_value()) {
          rs_idx_results.results_csi_rs_idxes.emplace(
              1, rrc_results_per_csi_rs_idx{1, meas_res.cell_results.results_csi_rs_cell});
        }
        meas_res.rs_idx_results = rs_idx_results;
      }
    }
  }
  return pos_info;
}

void cell_meas_manager::store_measurement_results(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results)
{
  if (ue_mng.find_ue(ue_index) == nullptr) {
    logger.info("ue={}: Not storing positioning measurement. Cause: UE not found", ue_index);
  }

  cell_meas_manager_ue_context& ue_meas_context = ue_mng.get_measurement_context(ue_index);

  expected<cell_measurement_positioning_info, std::string> pos_info = generate_measurement_objects_for_positioning(
      ue_index, ue_mng, cfg, nci_to_serving_cell_meas_config, meas_results, logger);

  if (!pos_info.has_value()) {
    logger.info("ue={}: Not storing positioning measurement. Cause: {}", ue_index, pos_info.error());
    return;
  }

  // Store measurement results in measurement context.
  logger.info("ue={}: Storing positioning measurement", ue_index);
  ue_meas_context.meas_results = pos_info.value();
}
