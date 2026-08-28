// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "config/sched_config_manager.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/time_alignment_config.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"

namespace ocudu {

/// \brief Interface used to notify the outcome of an event that a cell handled for one of its UEs.
///
/// It carries the outcomes whose handling needs the state shared by the UEs of the cell group, which the cell
/// scheduler has no access to.
class cell_ue_event_notifier
{
public:
  virtual ~cell_ue_event_notifier() = default;

  /// The Msg3 of a contention-free access was ACKed, which completes the contention resolution of the UE.
  virtual void on_cfra_msg3_acked(du_ue_index_t ue_index) = 0;

  /// The contention resolution CE of the UE was ACKed.
  virtual void on_conres_ce_acked(du_ue_index_t ue_index) = 0;

  /// A scheduling request of the UE was detected in the given UCI slot.
  virtual void on_sr_detected(du_ue_index_t ue_index, slot_point uci_slot) = 0;

  /// An N_TA update was measured for the time alignment group of the UE in this cell.
  virtual void on_ul_n_ta_update(du_ue_index_t              ue_index,
                                 time_alignment_group::id_t tag_id,
                                 phy_time_unit              n_ta_diff,
                                 float                      ul_sinr) = 0;
};

/// \brief Interface used to apply to a cell group the UE configuration requests that a cell dispatched.
///
/// The methods return whether the request was applied, so that the cell that dispatched it can report the failure.
class cell_group_ue_config_handler
{
public:
  virtual ~cell_group_ue_config_handler() = default;

  /// Create a UE in the cell group.
  virtual bool handle_ue_creation(ue_config_update_event ev) = 0;

  /// Reconfigure a UE of the cell group.
  virtual bool handle_ue_reconfiguration(ue_config_update_event ev) = 0;

  /// Delete a UE of the cell group.
  virtual bool handle_ue_deletion(ue_config_delete_event ev) = 0;

  /// Confirm that the UE applied the last configuration sent to it.
  virtual bool handle_ue_config_applied(du_ue_index_t ue_index) = 0;

  /// Deactivate a UE of the cell group.
  virtual bool handle_ue_deactivation_request(du_ue_index_t ue_index) = 0;

  /// Apply a slice reconfiguration to the cell group.
  virtual void handle_slice_reconfiguration(const du_cell_slice_reconfig_request& req) = 0;
};

/// \brief Interface through which a cell notifies the events that its cell group has to handle.
///
/// The cell dispatches them; the cell group handles them synchronously.
class cell_group_event_notifier : public cell_ue_event_notifier
{
public:
  /// Create a UE in the cell group.
  virtual bool on_ue_creation(ue_config_update_event ev) = 0;

  /// Reconfigure a UE of the cell group.
  virtual bool on_ue_reconfiguration(ue_config_update_event ev) = 0;

  /// Delete a UE of the cell group.
  virtual bool on_ue_deletion(ue_config_delete_event ev) = 0;

  /// The UE applied the last configuration sent to it.
  virtual bool on_ue_config_applied(du_ue_index_t ue_idx) = 0;

  /// The deactivation of the UE was requested.
  virtual bool on_ue_deactivation_request(du_ue_index_t ue_idx) = 0;

  /// A buffer status report of the UE was received.
  virtual void on_ul_bsr_indication(const ul_bsr_indication_message& bsr) = 0;

  /// A power headroom report of the UE was received.
  virtual void on_ul_phr_indication(const ul_phr_indication_message& phr) = 0;

  /// A timing advance report of the UE was received.
  virtual void on_ul_ta_report_indication(const ul_ta_report_indication_message& ta_report) = 0;

  /// A MAC CE is pending transmission to the UE.
  virtual void on_dl_mac_ce_indication(const dl_mac_ce_indication& ce) = 0;

  /// A C-RNTI MAC CE of the UE was received.
  virtual void on_crnti_ce_received(du_ue_index_t ue_index) = 0;

  /// The slices of the cell were reconfigured.
  virtual void on_slice_reconfiguration(const du_cell_slice_reconfig_request& req) = 0;
};

/// \brief Relays the notifications of a cell to the UE scheduler.
///
/// It is handed to both sides at their creation, so that the cell can notify without knowing whether the UE
/// scheduler cell that consumes the notifications exists yet.
class cell_ue_event_relay final : public cell_group_event_notifier
{
public:
  /// Set the handlers that the cell notifications and UE configuration requests are relayed to.
  void connect(cell_ue_event_notifier&       notifier,
               cell_group_ue_config_handler& ue_configurator,
               ue_feedback_handler&          ue_fb_handler)
  {
    handler        = &notifier;
    ue_cfg_handler = &ue_configurator;
    fb_handler     = &ue_fb_handler;
  }

  void on_cfra_msg3_acked(du_ue_index_t ue_index) override
  {
    if (handler != nullptr) {
      handler->on_cfra_msg3_acked(ue_index);
    }
  }

  void on_conres_ce_acked(du_ue_index_t ue_index) override
  {
    if (handler != nullptr) {
      handler->on_conres_ce_acked(ue_index);
    }
  }

  void on_sr_detected(du_ue_index_t ue_index, slot_point uci_slot) override
  {
    if (handler != nullptr) {
      handler->on_sr_detected(ue_index, uci_slot);
    }
  }

  bool on_ue_creation(ue_config_update_event ev) override
  {
    return ue_cfg_handler != nullptr and ue_cfg_handler->handle_ue_creation(std::move(ev));
  }

  bool on_ue_reconfiguration(ue_config_update_event ev) override
  {
    return ue_cfg_handler != nullptr and ue_cfg_handler->handle_ue_reconfiguration(std::move(ev));
  }

  bool on_ue_deletion(ue_config_delete_event ev) override
  {
    return ue_cfg_handler != nullptr and ue_cfg_handler->handle_ue_deletion(std::move(ev));
  }

  bool on_ue_config_applied(du_ue_index_t ue_idx) override
  {
    return ue_cfg_handler != nullptr and ue_cfg_handler->handle_ue_config_applied(ue_idx);
  }

  bool on_ue_deactivation_request(du_ue_index_t ue_idx) override
  {
    return ue_cfg_handler != nullptr and ue_cfg_handler->handle_ue_deactivation_request(ue_idx);
  }

  void on_ul_n_ta_update(du_ue_index_t              ue_index,
                         time_alignment_group::id_t tag_id,
                         phy_time_unit              n_ta_diff,
                         float                      ul_sinr) override
  {
    if (handler != nullptr) {
      handler->on_ul_n_ta_update(ue_index, tag_id, n_ta_diff, ul_sinr);
    }
  }

  void on_ul_bsr_indication(const ul_bsr_indication_message& bsr) override
  {
    if (fb_handler != nullptr) {
      fb_handler->handle_ul_bsr_indication(bsr);
    }
  }

  void on_ul_phr_indication(const ul_phr_indication_message& phr) override
  {
    if (fb_handler != nullptr) {
      fb_handler->handle_ul_phr_indication(phr);
    }
  }

  void on_ul_ta_report_indication(const ul_ta_report_indication_message& ta_report) override
  {
    if (fb_handler != nullptr) {
      fb_handler->handle_ul_ta_report_indication(ta_report);
    }
  }

  void on_dl_mac_ce_indication(const dl_mac_ce_indication& ce) override
  {
    if (fb_handler != nullptr) {
      fb_handler->handle_dl_mac_ce_indication(ce);
    }
  }

  void on_crnti_ce_received(du_ue_index_t ue_index) override
  {
    if (fb_handler != nullptr) {
      fb_handler->handle_crnti_ce_received(ue_index);
    }
  }

  void on_slice_reconfiguration(const du_cell_slice_reconfig_request& req) override
  {
    if (ue_cfg_handler != nullptr) {
      ue_cfg_handler->handle_slice_reconfiguration(req);
    }
  }

private:
  cell_ue_event_notifier*       handler        = nullptr;
  cell_group_ue_config_handler* ue_cfg_handler = nullptr;
  ue_feedback_handler*          fb_handler     = nullptr;
};

} // namespace ocudu
