// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_cell.h"
#include "ocudu/adt/flat_map.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/serv_cell_index.h"

namespace ocudu {

class cell_metrics_handler;

/// Container that stores all the UEs that are configured in a given cell.
class ue_cell_repository
{
  using ue_list = slotted_id_vector<du_ue_index_t, free_list_object_pool<ue_cell>::ptr>;

public:
  ue_cell_repository(const cell_configuration& cell_cfg, cell_metrics_handler* cell_metrics);

  du_cell_index_t cell_index() const { return cell_idx; }

  /// Metrics handler for this cell (may be nullptr if metrics are disabled).
  cell_metrics_handler* get_metrics() const { return metrics; }
  bool                  contains(du_ue_index_t ue_index) const { return ues.contains(ue_index); }
  bool   contains(rnti_t rnti) const { return rnti_to_ue_index_lookup.find(rnti) != rnti_to_ue_index_lookup.end(); }
  size_t size() const { return ues.size(); }
  bool   empty() const { return ues.empty(); }

  ue_cell&       operator[](du_ue_index_t ue_index) { return *ues[ue_index]; }
  const ue_cell& operator[](du_ue_index_t ue_index) const { return *ues[ue_index]; }
  ue_cell*       find(du_ue_index_t ue_index) { return ues.contains(ue_index) ? ues[ue_index].get() : nullptr; }
  const ue_cell* find(du_ue_index_t ue_index) const { return ues.contains(ue_index) ? ues[ue_index].get() : nullptr; }
  ue_cell*       find_by_rnti(rnti_t rnti)
  {
    auto it = rnti_to_ue_index_lookup.find(rnti);
    return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
  }
  const ue_cell* find_by_rnti(rnti_t rnti) const
  {
    auto it = rnti_to_ue_index_lookup.find(rnti);
    return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
  }

  /// Get HARQs managed by this cell.
  cell_harq_manager& get_cell_harqs() { return cell_harqs; }

  /// Update last processed slot for this cell.
  void slot_indication(slot_point sl_tx);

  /// Stop all UE-related operations in this cell repository.
  void deactivate();

private:
  friend class ue_repository;

  /// Add a new UE to the UE cell repository.
  ue_cell& add_ue(const ue_configuration& ue_cfg,
                  serv_cell_index_t       serv_cell_index,
                  ue_pcell_state*         ue_pcell_fsm,
                  ue_shared_context       shared_ctx);

  void rem_ue(du_ue_index_t ue_index);

  const du_cell_index_t   cell_idx;
  cell_metrics_handler*   metrics;
  ocudulog::basic_logger& logger;

  /// HARQs manager for the cell.
  cell_harq_manager cell_harqs;

  // Note: The pools are declared before the list of UEs, as the UEs hold objects taken from them.
  // Note: The separate pools for different components are used for memory access efficiency (SoA).

  /// Pool of UE cells of this cell.
  free_list_object_pool<ue_cell> ue_pool;

  /// Pool of channel state managers of the cell.
  free_list_object_pool<ue_channel_state_manager> channel_state_pool;

  /// Pool of link adaptation controllers of the cell.
  free_list_object_pool<ue_link_adaptation_controller> mcs_calculator_pool;

  /// Pools of PUSCH and PUCCH power controllers of the cell.
  free_list_object_pool<pusch_power_controller> pusch_pwr_controller_pool;
  free_list_object_pool<pucch_power_controller> pucch_pwr_controller_pool;

  // List of UEs in the cell.
  ue_list ues;

  // Mapping of RNTIs to UE indexes.
  flat_map<rnti_t, du_ue_index_t> rnti_to_ue_index_lookup;
};

} // namespace ocudu
