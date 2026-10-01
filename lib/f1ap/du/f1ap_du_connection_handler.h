// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1ap_du_connection_manager.h"
#include "ocudu/f1ap/f1ap_message_handler.h"
#include "ocudu/f1ap/gateways/f1c_connection_client.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {
namespace odu {

class f1ap_du_configurator;
struct f1ap_du_context;

/// \brief Handler of TNL connection between DU and CU-CP.
class f1ap_du_connection_handler
{
public:
  f1ap_du_connection_handler(f1c_connection_client& f1c_client_handler_,
                             f1ap_message_handler&  f1ap_pdu_handler_,
                             f1ap_du_configurator&  du_mng_,
                             f1ap_du_context&       du_ctxt_,
                             task_executor&         ctrl_exec_);
  ~f1ap_du_connection_handler();

  [[nodiscard]] std::unique_ptr<f1ap_message_notifier> connect_to_cu_cp();

  async_task<void> handle_tnl_association_removal();

  /// \brief Check if the connection is active.
  [[nodiscard]] bool is_connected() const { return connected_flag; }

private:
  // Called by the F1-C GW when the F1-C TNL association drops.
  void handle_connection_loss();

  void handle_connection_loss_impl();

  f1c_connection_client&  f1c_client_handler;
  f1ap_message_handler&   f1ap_pdu_handler;
  f1ap_du_configurator&   du_mng;
  f1ap_du_context&        du_ctxt;
  task_executor&          ctrl_exec;
  ocudulog::basic_logger& logger;

  bool connected_flag{false};

  std::unique_ptr<f1ap_message_notifier> tx_pdu_notifier;
  unsigned                               f1c_session_epoch{0};

  manual_event_flag rx_path_disconnected;
};

} // namespace odu
} // namespace ocudu
