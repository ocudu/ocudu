// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "logical_channel_system.h"
#include "ta_management_system.h"
#include "ue_cell.h"
#include "ue_drx_controller.h"
#include "ue_ta_report_tracker.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/serv_cell_index.h"
#include "ocudu/scheduler/mac_scheduler.h"

namespace ocudu {

/// \brief Structure to lookup UE cells based on DU cell index or UE-specific cell index.
struct ue_cell_lookup {
  /// List of UE cells indexed by \c du_cell_index_t.
  slotted_id_table<du_cell_index_t, ue_cell*, MAX_NOF_DU_CELLS> du_cells;
  /// List of UE cells indexed by \c serv_cell_index_t. The size of the list is equal to the number of cells
  /// aggregated and configured for the UE. PCell corresponds to serv_cell_index=0. The first SCell corresponds to
  /// serv_cell_index=1.
  static_vector<ue_cell*, MAX_NOF_DU_CELLS> ue_cells;
};

/// UE handle used by the MAC scheduler to manage UE-specific state and operations.
class ue
{
public:
  ue(const ue_configuration&       cfg_,
     ue_logical_channel_repository dl_lch_repo,
     ue_drx_controller&            drx_ctrl,
     ue_ta_report_tracker&         ta_report_tracker_,
     ue_ta_manager                 ta_mgr_,
     const ue_cell_lookup&         ue_cells);
  ue(const ue&)            = delete;
  ue(ue&&)                 = delete;
  ue& operator=(const ue&) = delete;
  ue& operator=(ue&&)      = delete;

  const du_ue_index_t ue_index;
  const rnti_t        crnti;

  void slot_indication(slot_point sl_tx);

  void deactivate();

  ue_cell*       find_cell(du_cell_index_t cell_index) { return cells.du_cells[cell_index]; }
  const ue_cell* find_cell(du_cell_index_t cell_index) const { return cells.du_cells[cell_index]; }

  /// \brief Fetch UE cell based on UE-specific cell identifier. E.g. PCell corresponds to serv_cell_index==0.
  ue_cell&       get_cell(serv_cell_index_t serv_cell_index) { return *cells.ue_cells[serv_cell_index]; }
  const ue_cell& get_cell(serv_cell_index_t serv_cell_index) const { return *cells.ue_cells[serv_cell_index]; }

  /// \brief Fetch UE PCell.
  ue_cell&       get_pcell() { return *cells.ue_cells[0]; }
  const ue_cell& get_pcell() const { return *cells.ue_cells[0]; }

  /// \brief Number of cells configured for the UE.
  unsigned nof_cells() const { return cells.ue_cells.size(); }

  /// \brief Returns dedicated configuration for the UE.
  const ue_configuration* ue_cfg_dedicated() const { return ue_ded_cfg; }

  bool is_ca_enabled() const { return cells.ue_cells.size() > 1; }

  void activate_cells(bounded_bitset<MAX_NOF_DU_CELLS> activ_bitmap) {}

  /// \brief Handle received SR indication.
  void handle_sr_indication(slot_point uci_slot) { lc_ch_mgr.handle_sr_indication(uci_slot); }

  /// \brief Handles received BSR indication by updating UE UL logical channel states.
  void handle_bsr_indication(const ul_bsr_indication_message& msg) { lc_ch_mgr.handle_bsr_indication(msg); }

  /// \brief Handles MAC CE indication.
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& msg)
  {
    lc_ch_mgr.handle_mac_ce_indication({.ce_lcid = msg.ce_lcid, .ce_payload = dummy_ce_payload{}});
  }

  /// Called when a new UE configuration is passed to the scheduler, as part of the RRC Reconfiguration procedure.
  void handle_reconfiguration_request(const ue_configuration& new_cfg);

  /// Handle a received N_TA update indication by forwarding it to the Timing Advance manager of the UE.
  void handle_ul_n_ta_update_indication(time_alignment_group::id_t tag_id, phy_time_unit n_ta_diff, float ul_sinr)
  {
    ta_mgr.handle_ul_n_ta_update_indication(tag_id, n_ta_diff.to_Tc(), ul_sinr);
  }

  /// \brief Handles DL Buffer State indication.
  void handle_dl_buffer_state_indication(lcid_t lcid, unsigned bs, slot_point hol_toa = {});

  /// \brief Computes the number of UL pending bytes that are not already allocated in a UL HARQ. The value is used
  /// to derive the required transport block size for an UL grant.
  units::bytes pending_ul_newtx_bytes() const;

  /// \brief Retrieves UE DRX controller.
  ue_drx_controller& drx_controller() { return drx; }

  /// \brief Retrieves the tracker of the uplink timing advance reported by the UE.
  ue_ta_report_tracker& ta_report_tracker() { return ta_report; }

  /// Retrieve UE logical channel manager.
  const ue_logical_channel_repository& logical_channels() const { return lc_ch_mgr; }
  ue_logical_channel_repository&       logical_channels() { return lc_ch_mgr; }

private:
  // Expert config parameters used for UE scheduler.
  const scheduler_ue_expert_config& expert_cfg;
  // Cell configuration. This is common to all UEs within the same cell.
  const cell_configuration& cell_cfg_common;
  // Dedicated configuration for the UE.
  const ue_configuration* ue_ded_cfg = nullptr;
  /// UE Logical Channel Manager.
  ue_logical_channel_repository lc_ch_mgr;
  /// Controller of DRX active timer.
  ue_drx_controller& drx;
  /// Tracker of the uplink timing advance reported by the UE.
  ue_ta_report_tracker& ta_report;
  /// Timing Advance manager of the UE.
  ue_ta_manager ta_mgr;
  /// Configured cells for the UE.
  const ue_cell_lookup& cells;

  slot_point last_sl_tx;
};

} // namespace ocudu
