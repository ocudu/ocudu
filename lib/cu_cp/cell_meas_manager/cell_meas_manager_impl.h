// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../ue_manager/ue_manager_impl.h"
#include "ocudu/adt/span.h"
#include "ocudu/cu_cp/cell_meas_manager_config.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/meas_types.h"
#include "ocudu/ran/plmn_identity.h"
#include <unordered_map>

namespace ocudu::ocucp {

/// Methods used by cell measurement manager to signal measurement events to the mobility manager.
class cell_meas_mobility_manager_notifier
{
public:
  virtual ~cell_meas_mobility_manager_notifier() = default;

  /// \brief Notifies that a neighbor cell became stronger than the current serving cell.
  virtual void on_neighbor_better_than_spcell(cu_cp_ue_index_t     ue_index,
                                              gnb_id_t             neighbor_gnb_id,
                                              nr_cell_identity     neighbor_nci,
                                              pci_t                neighbor_pci,
                                              plmn_identity        neighbor_plmn,
                                              std::optional<tac_t> neighbor_tac = std::nullopt) = 0;
};

struct cell_meas_manager_dependencies {
  cell_meas_mobility_manager_notifier& mobility_mng_notifier;
  ue_manager&                          ue_mng;
  ocudulog::basic_logger&              logger;
};

/// Basic cell manager implementation
class cell_meas_manager
{
public:
  cell_meas_manager(const cell_meas_manager_config& cfg_, const cell_meas_manager_dependencies& dependencies);
  ~cell_meas_manager() = default;

  std::optional<rrc_meas_cfg>
                                  get_measurement_config(cu_cp_ue_index_t                   ue_index,
                                                         nr_cell_identity                   nci,
                                                         const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt,
                                                         bool                               cond_meas      = false,
                                                         span<const pci_t>                  candidate_pcis = {});
  std::optional<cell_meas_config> get_cell_config(nr_cell_identity nci);
  std::vector<pci_t>              get_neighbor_pcis(nr_cell_identity serving_nci) const;
  /// \brief Add a cell or replace its serving-cell parameters as a whole: optional parameters left unset in
  /// \c serv_cell_cfg are cleared, and a cell that stops being complete is detached from its measurement object.
  /// The neighbor relations and the periodic report of the cell are kept.
  bool update_cell_config(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg);
  bool update_ntn_neighbour_info(nr_cell_identity serving_nci, span<const rrc_ntn_neighbour_cell_info_item> ncells);
  void report_measurement(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results);

  /// \brief Remove a cell and everything that references it: its measurement object attachment and the
  /// neighbor relations of other cells pointing at it.
  /// \return True if the cell existed and was removed.
  bool remove_cell_config(nr_cell_identity nci);

  /// \brief Add a neighbor relation between two configured cells, or replace the report config ids of an
  /// existing one.
  ///
  /// Both cells must be configured, the relation must not be reflexive, and every report config id must
  /// exist and not be of periodical type (periodical reports are serving-cell-only).
  /// \return True if the relation was added or updated.
  bool add_or_update_neighbor(nr_cell_identity             serving_nci,
                              nr_cell_identity             neighbor_nci,
                              std::vector<report_cfg_id_t> report_cfg_ids);

  /// \brief Remove the neighbor relation from \c serving_nci to \c neighbor_nci. Directional: the reverse
  /// relation, if any, is kept.
  /// \return True if the relation existed and was removed.
  bool remove_neighbor(nr_cell_identity serving_nci, nr_cell_identity neighbor_nci);

  /// \brief Add a new report configuration or replace an existing one (a measurement profile referenced by
  /// neighbor relations and serving-cell periodic reports).
  ///
  /// A config referenced by a neighbor relation cannot be replaced with a periodical one, and a config
  /// referenced as a serving-cell periodic report must stay periodical.
  /// \return True if the report configuration was added or updated.
  bool add_or_update_report_config(report_cfg_id_t report_cfg_id, const rrc_report_cfg_nr& report_cfg);

  /// \brief Remove a report configuration. Refused while any neighbor relation or serving-cell periodic
  /// report references it.
  /// \return True if the report configuration existed and was removed.
  bool remove_report_config(report_cfg_id_t report_cfg_id);

  /// \brief Set or clear the serving-cell periodical report config of a cell. The id must reference an
  /// existing report configuration of periodical type.
  /// \return True if the cell exists and the periodic report was updated.
  bool set_periodic_report_config(nr_cell_identity nci, std::optional<report_cfg_id_t> report_cfg_id);

  expected<std::pair<unsigned, nr_cell_identity>> find_neighbour_nci(pci_t pci);

private:
  /// \brief Generate measurement objects for the given cell configuration.
  void generate_measurement_objects_for_serving_cells();

  void update_measurement_object(nr_cell_identity nci, const serving_cell_meas_config& serving_cell_cfg);

  /// \brief Detach a cell from the measurement object lookups, dropping the frequency's measurement object
  /// when the cell was the last one attached to it. No-op for an unknown cell.
  void remove_measurement_object(nr_cell_identity nci);

  /// \brief Build a config that only removes the UE's current measurement config and drop the UE's
  /// measurement id bookkeeping. Returns nullopt when the UE has nothing to remove.
  std::optional<rrc_meas_cfg> remove_current_meas_config(cu_cp_ue_index_t                   ue_index,
                                                         const std::optional<rrc_meas_cfg>& current_meas_config);

  void store_measurement_results(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results);

  cell_meas_manager_config             cfg;
  cell_meas_mobility_manager_notifier& mobility_mng_notifier;
  ue_manager&                          ue_mng;

  std::unordered_map<ssb_frequency_t, rrc_meas_obj_nr>
      ssb_freq_to_meas_object; // unique measurement objects, indexed by SSB frequency.
  std::unordered_map<ssb_frequency_t, std::vector<nr_cell_identity>> ssb_freq_to_ncis;
  std::map<nr_cell_identity, serving_cell_meas_config>               nci_to_serving_cell_meas_config;

  ocudulog::basic_logger& logger;
};

} // namespace ocudu::ocucp
