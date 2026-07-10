// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
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
  bool update_cell_config(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg);
  bool update_ntn_neighbour_info(nr_cell_identity serving_nci, span<const rrc_ntn_neighbour_cell_info_item> ncells);
  void report_measurement(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results);

  expected<std::pair<unsigned, nr_cell_identity>> find_neighbour_nci(pci_t pci);

private:
  /// \brief Generate measurement objects for the given cell configuration.
  void generate_measurement_objects_for_serving_cells();

  void update_measurement_object(nr_cell_identity nci, const serving_cell_meas_config& serving_cell_cfg);

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
