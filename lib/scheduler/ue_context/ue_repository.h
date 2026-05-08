// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/sched_config_manager.h"
#include "logical_channel_system.h"
#include "ta_management_system.h"
#include "ue.h"
#include "ue_cell_repository.h"
#include "ue_drx_controller.h"
#include "ue_fsm_states.h"
#include "ocudu/adt/ring_buffer.h"

namespace ocudu {

/// Container that stores all scheduler UEs.
class ue_repository
{
  using ue_list = slotted_id_table<du_ue_index_t, ue, MAX_NOF_DU_UES>;

public:
  using value_type     = ue_list::value_type;
  using iterator       = ue_list::iterator;
  using const_iterator = ue_list::const_iterator;

  explicit ue_repository(const scheduler_ue_expert_config& cfg);
  ~ue_repository();

  /// \brief Mark start of new slot and update UEs states.
  void slot_indication(slot_point sl_tx);

  /// \brief Contains ue index.
  bool contains(du_ue_index_t ue_index) const { return ues.contains(ue_index); }

  ue&       operator[](du_ue_index_t ue_index) { return ues[ue_index]; }
  const ue& operator[](du_ue_index_t ue_index) const { return ues[ue_index]; }

  ue_cell_repository& add_cell(const cell_configuration& cell_cfg, cell_metrics_handler* cell_metrics);
  void                rem_cell(du_cell_index_t cell_index);

  // Access UEs per cell.
  ue_cell_repository&       cell(du_cell_index_t cell_index) { return *cell_ues[cell_index]; }
  const ue_cell_repository& cell(du_cell_index_t cell_index) const { return *cell_ues[cell_index]; }

  /// \brief Search UE context based on TC-RNTI/C-RNTI.
  ue*       find_by_rnti(rnti_t rnti);
  const ue* find_by_rnti(rnti_t rnti) const;

  /// \brief Add new UE in the UE repository.
  void add_ue(const ue_configuration& ue_cfg, bool starts_in_fallback, std::optional<slot_point> ul_ccch_slot_rx);

  /// \brief Reconfigure existing UE.
  void reconfigure_ue(const ue_configuration& new_cfg, sched_ue_config_request::causes cause);

  /// \brief Called when UE configuration has been applied (e.g. after RRC ReconfigurationComplete).
  bool ue_config_applied(du_ue_index_t ue_index);

  /// \brief Called when C-RNTI CE is received.
  bool crnti_ce_received(du_ue_index_t ue_index);

  /// \brief Called when C-RNTI CE is received.
  bool handle_conres_ce_outcome(du_ue_index_t ue_index, bool success);

  /// \brief Initiate removal of existing UE from the repository.
  void schedule_ue_rem(ue_config_delete_event ev);

  bounded_bitset<MAX_NOF_DU_UES> get_ues_with_pending_newtx_data(ran_slice_id_t slice_id, bool is_dl) const;

  ue*       find(du_ue_index_t ue_index) { return ues.contains(ue_index) ? &ues[ue_index] : nullptr; }
  const ue* find(du_ue_index_t ue_index) const { return ues.contains(ue_index) ? &ues[ue_index] : nullptr; }

  size_t size() const { return ues.size(); }

  bool empty() const { return ues.empty(); }

  iterator       begin() { return ues.begin(); }
  iterator       end() { return ues.end(); }
  const_iterator begin() const { return ues.begin(); }
  const_iterator end() const { return ues.end(); }

  const_iterator lower_bound(du_ue_index_t ue_index) const { return ues.lower_bound(ue_index); }

  /// Handle cell deactivation by removing all UEs that are associated with the cell.
  void handle_cell_deactivation(du_cell_index_t cell_index);

private:
  /// Force the removal of the UE without waiting for the flushing of pending events.
  void rem_ue(const ue& u);

  /// Update UE config FSM.
  bool update_ue_fsm(du_ue_index_t ue_index, ue_fsm_config_event ev);

  ocudulog::basic_logger& logger;

  // List of UEs per cell.
  slotted_id_table<du_cell_index_t, std::unique_ptr<ue_cell_repository>, MAX_NOF_DU_CELLS> cell_ues;

  /// FSM of the UE configuration stages.
  slotted_id_table<du_ue_index_t, ue_pcell_state, MAX_NOF_DU_UES> ue_fsms;

  /// Management of all UE logical channels.
  logical_channel_system lc_ch_sys;

  /// DRX controllers per UE.
  slotted_id_table<du_ue_index_t, ue_drx_controller, MAX_NOF_DU_UES, false> ue_drx_controllers;

  /// UE Timing Advance Manager.
  ta_management_system ta_mgr_sys;

  /// Lookup of UE cells per UE.
  slotted_id_table<du_ue_index_t, ue_cell_lookup, MAX_NOF_DU_UES, false> ue_cell_lookups;

  /// Repository of UE objects.
  ue_list ues;

  // Mapping of RNTIs to UE indexes.
  flat_map<rnti_t, du_ue_index_t> rnti_to_ue_index_lookup;

  // Queue of UEs marked for later removal. For each UE, we store the slot after which its removal can be safely
  // carried out, and the original UE removal command.
  ring_buffer<std::pair<slot_point, ue_config_delete_event>> ues_to_rem{MAX_NOF_DU_UES};

  // Last slot indication.
  slot_point last_sl_tx;
};

} // namespace ocudu
