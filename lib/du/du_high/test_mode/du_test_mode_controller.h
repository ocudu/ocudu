// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/f1ap/gateways/f1c_connection_client.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/timers.h"
#include <memory>
#include <vector>

namespace ocudu {
class mac_pdu_handler;
class task_executor;
} // namespace ocudu

namespace ocudu::odu {

class f1ap_du;

/// Controls the UE attach/detach cycling lifecycle in DU test mode.
///
/// Per cell, the cycle is: create all UEs → wait until all established → run traffic for attach_detach_duration ms →
/// release all UEs → guard period (1 second) → repeat.
///
/// The controller wraps the f1c_connection_client (to intercept outgoing F1AP messages and capture gnb_du_ue_f1ap_id),
/// and provides mac_cell_result_notifier wrappers per cell (to observe Msg4 events and slot timing).
///
/// All internal state is accessed on ctrl_exec. Cross-thread notifications are dispatched via ctrl_exec.defer().
class du_test_mode_controller
{
  class f1c_wrapper_impl;
  class cell_notifier_impl;
  /// DU test mode cell-specific state handler.
  class cell_controller;

  friend class cell_controller;

public:
  du_test_mode_controller(const du_test_mode_config::test_mode_ue_config& cfg_,
                          timer_manager&                                  timers_,
                          task_executor&                                  ctrl_exec_,
                          unsigned                                        nof_cells_);
  ~du_test_mode_controller();

  /// Set the f1c upstream (f1ap_testmode). Must be called before real f1ap connects.
  void set_f1c_upstream(f1c_connection_client& upstream);

  /// Returns the f1c_connection_client wrapper to pass to the real f1ap creation.
  f1c_connection_client& get_f1c_wrapper();

  /// Returns a mac_cell_result_notifier wrapper for the given cell, wrapping \p real_phy_notifier.
  mac_cell_result_notifier& add_cell_notifier(du_cell_index_t cell_index, mac_cell_result_notifier& real_phy_notifier);

  /// Set MAC pdu handler and F1AP handler. Called after MAC and F1AP are created.
  void connect(mac_pdu_handler& pdu_handler_, f1ap_du& f1ap_);

  // Called from cell thread (dispatches to ctrl_exec internally):
  void handle_conres_scheduled(du_cell_index_t cell_index, rnti_t rnti);
  void handle_slot_completed(du_cell_index_t cell_index, slot_point slot);

  // Called from F1AP tx interceptor thread (dispatches to ctrl_exec internally):
  void on_ue_f1ap_id_captured(rnti_t rnti, gnb_du_ue_f1ap_id_t gnb_du_ue_id);

  // Called on ctrl_exec from mac_test_mode_adapter::handle_ue_delete_request:
  void on_ue_removed(rnti_t rnti);

private:
  struct ue_entry {
    rnti_t              rnti;
    gnb_du_ue_f1ap_id_t gnb_du_ue_id;
  };

  // Initiate the UE Context Release Command from the F1AP interface.
  bool release_ue(rnti_t rnti);

  bool is_test_ue_in_cell(du_cell_index_t cell_index, rnti_t rnti) const
  {
    const unsigned base = to_value(cfg.rnti) + static_cast<unsigned>(cell_index) * cfg.nof_ues;
    const unsigned v    = to_value(rnti);
    return v >= base and v < base + cfg.nof_ues;
  }

  const du_test_mode_config::test_mode_ue_config& cfg;
  task_executor&                                  ctrl_exec;
  mac_pdu_handler*                                pdu_handler  = nullptr;
  f1ap_du*                                        f1ap_handler = nullptr;
  ocudulog::basic_logger&                         logger;

  std::vector<ue_entry>                            ue_id_table;
  std::vector<std::unique_ptr<cell_controller>>    cells;
  std::unique_ptr<f1c_wrapper_impl>                f1c_wrapper;
  std::vector<std::unique_ptr<cell_notifier_impl>> cell_notifiers;
};

} // namespace ocudu::odu
