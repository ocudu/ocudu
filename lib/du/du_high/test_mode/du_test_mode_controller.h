// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "f1c_du_test_mode_adapter.h"
#include "mac_test_mode_adapter.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
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

/// \brief Controls the UE creation/destruction in DU test mode.
///
/// Two modes of operation supported:
/// - stable traffic - A fixed number of UEs is created per cell, and traffic flows until shutdown.
/// - cyclic attach/detach - Per cell, the cycle is: create all UEs → wait until all established → run traffic for
/// attach_detach_duration → release all UEs → guard period → repeat.
///
/// The controller wraps the f1c_connection_client (to intercept outgoing F1AP messages and capture gnb_du_ue_f1ap_id),
/// and wraps the MAC to intercept events coming in and out of it.
///
/// All internal state is accessed on ctrl_exec. Cross-thread notifications are dispatched via ctrl_exec.defer().
class du_test_mode_controller
{
  /// DU test mode cell-specific state handler.
  class cell_controller;
  /// Notifier of MAC events to controller.
  class mac_event_notifier;
  /// Notifier of F1-c events to controller.
  class f1c_event_notifier;

public:
  du_test_mode_controller(const du_test_mode_config::test_mode_ue_config& cfg_,
                          f1c_connection_client&                          f1c_client_,
                          mac_result_notifier&                            phy_notifier_,
                          timer_manager&                                  timers_,
                          task_executor&                                  ctrl_exec_,
                          unsigned                                        nof_cells_);
  ~du_test_mode_controller();

  /// Returns the f1c_connection_client wrapper to inject into the real F1AP-DU creation.
  f1c_connection_client& get_f1c_client() { return f1c_adapter; }

  /// Returns the notifier that will be used by the MAC to send results to lower layers.
  mac_result_notifier& get_phy_notifier() const { return mac_ctrl.get_phy_notifier(); }

  /// Generate a wrapper of the MAC for test mode.
  std::unique_ptr<mac_interface> decorate(std::unique_ptr<mac_interface> mac_ptr);

private:
  bool is_test_ue_in_cell(du_cell_index_t cell_index, rnti_t rnti) const
  {
    const unsigned base = to_value(cfg.rnti) + static_cast<unsigned>(cell_index) * cfg.nof_ues;
    const unsigned v    = to_value(rnti);
    return v >= base and v < base + cfg.nof_ues;
  }

  void handle_ue_removed(rnti_t rnti);

  const du_test_mode_config::test_mode_ue_config& cfg;
  task_executor&                                  ctrl_exec;
  ocudulog::basic_logger&                         logger;

  /// Adapter of the F1-c client.
  f1c_du_test_mode_adapter f1c_adapter;

  /// Adapter of MAC.
  mac_test_mode_adapter mac_ctrl;

  std::vector<std::unique_ptr<cell_controller>> cells;
};

} // namespace ocudu::odu
