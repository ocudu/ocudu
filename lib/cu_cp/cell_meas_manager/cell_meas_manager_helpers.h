// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "measurement_context.h"
#include "ocudu/adt/span.h"
#include "ocudu/cu_cp/cell_meas_manager_config.h"
#include "ocudu/ran/meas_types.h"
#include "ocudu/rrc/rrc_ue.h"
#include <unordered_map>
#include <vector>

namespace ocudu::ocucp {

/// \brief Log configured cells.
void log_cells(const ocudulog::basic_logger& logger, const cell_meas_manager_config& cfg);

/// \brief Log measurement objects.
void log_meas_objects(const ocudulog::basic_logger&                               logger,
                      const std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>& meas_objects);

/// \brief Add old measurement configuration to the remove list of the new configuration.
/// \param[in] old_cfg The old configuration.
/// \param[out] new_cfg The new configuration.
void add_old_meas_config_to_rem_list(const rrc_meas_cfg& old_cfg, rrc_meas_cfg& new_cfg);

/// \brief Generate measurement objects for the given cell configuration.
/// \param[in] cfg The cell configuration.
/// \param[in] nci The cell id.
/// \returns A vector of SSB frequencies that correlate to measurement objects.
std::vector<ssb_frequency_t> generate_measurement_object_list(const cell_meas_manager_config& cfg,
                                                              nr_cell_identity                serving_nci);

/// \brief Generate unique SSB frequency list for CHO (conditional) measurement config.
/// Returns the set of SSB frequencies of neighbor cells whose PCI is in
/// \p candidate_pcis (all neighbors when \p candidate_pcis is empty)
/// and whose configuration is complete.
/// \param[in] cfg The cell measurement manager configuration.
/// \param[in] serving_nci The serving cell NCI.
/// \param[in] candidate_pcis Optional PCI filter; empty means all neighbors are included.
/// \returns Vector of unique SSB frequencies.
std::vector<ssb_frequency_t> generate_cho_measurement_object_list(const cell_meas_manager_config& cfg,
                                                                  nr_cell_identity                serving_nci,
                                                                  span<const pci_t>               candidate_pcis);

/// \brief Generate report configuration for the given cell configuration.
/// \param[in] cfg The cell configuration.
/// \param[in] nci The cell id.
/// \param[in] report_cfg_id The report configuration id.
/// \param[out] meas_cfg The resulting measurement configuration.
/// \param[out] ue_meas_context The UE measurement context.
void generate_report_config(const cell_meas_manager_config& cfg,
                            nr_cell_identity                nci,
                            report_cfg_id_t                 report_cfg_id,
                            rrc_meas_cfg&                   meas_cfg,
                            cell_meas_manager_ue_context&   ue_meas_context);

/// \brief Collect rrc_cond_trigger_cfg report configs from \p cfg into \p meas_cfg, filtered by UE capabilities.
/// Appends matching entries to meas_cfg.report_cfg_to_add_mod_list.
/// The number of collected triggers is capped at 1 when the UE does not support condHO-TwoTriggerEvents-r16,
/// and at 2 otherwise. Trigger configs requiring Rel-17 capabilities (event-A4, location, time) are skipped
/// if the UE does not advertise the corresponding capability.
/// \returns Vector of report_cfg_id_t for the collected conditional trigger configs, or empty if the UE does
///          not support CHO at all.
std::vector<report_cfg_id_t> collect_cond_trigger_report_configs(const cell_meas_manager_config&  cfg,
                                                                 rrc_meas_cfg&                    meas_cfg,
                                                                 const rrc_ue_capability_handler& ue_caps,
                                                                 ocudulog::basic_logger&          logger);

/// \brief Build measurement IDs linking MOs to conditional trigger report configs for CHO.
/// For each (mo_id, report_cfg_id) pair: allocates a meas_id, appends rrc_meas_id_to_add_mod,
/// populates meas_id_to_meas_context for every NCI that maps to that MO, and fills
/// meas_cfg.nci_to_meas_ids.
/// \returns false if any meas_id allocation fails, true otherwise.
bool generate_cho_meas_ids(const cell_meas_manager_config& cfg,
                           span<const report_cfg_id_t>     cond_trigger_ids,
                           rrc_meas_cfg&                   meas_cfg,
                           cell_meas_manager_ue_context&   ue_meas_context);

rrc_meas_obj_nr generate_measurement_object(const serving_cell_meas_config& cfg);

/// \brief Check whether the given measurement objects are the same.
bool is_duplicate(const rrc_meas_obj_nr& obj_1, const rrc_meas_obj_nr& obj_2);

} // namespace ocudu::ocucp
