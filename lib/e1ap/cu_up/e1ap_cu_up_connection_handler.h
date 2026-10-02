// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../common/e1ap_logger.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/e1ap/gateways/e1_connection_client.h"
#include "ocudu/support/async/manual_event.h"

namespace ocudu::ocuup {

/// Holds the E1AP CU-UP connection handler configuration parameters.
struct e1ap_cu_up_connection_handler_configuration {
  cu_up_e1_index_t e1_index;
};

/// Holds the E1AP CU-UP connection handler dependencies
struct e1ap_cu_up_connection_handler_dependencies {
  e1_connection_client&                   e1ap_client_handler;
  e1ap_message_handler&                   e1ap_pdu_handler;
  e1ap_cu_up_manager_connection_notifier& cu_up_manager;
  task_executor&                          cu_up_executor;
  e1ap_logger&                            logger;
};

class e1ap_cu_up_connection_handler
{
public:
  e1ap_cu_up_connection_handler(const e1ap_cu_up_connection_handler_configuration& cfg,
                                const e1ap_cu_up_connection_handler_dependencies&  dependencies);
  ~e1ap_cu_up_connection_handler();

  [[nodiscard]] e1ap_message_notifier* connect_to_cu_cp();
  [[nodiscard]] bool                   is_connected() const { return connected_flag; }

  async_task<void> handle_tnl_association_removal();

private:
  void handle_connection_loss(unsigned session);
  void handle_connection_loss_impl();

  cu_up_e1_index_t e1_index;

  e1_connection_client&                   e1_client_handler;
  e1ap_message_handler&                   e1ap_pdu_handler;
  e1ap_cu_up_manager_connection_notifier& cu_up_manager;
  task_executor&                          cu_up_executor;
  e1ap_logger&                            logger;

  std::unique_ptr<e1ap_message_notifier> e1ap_notifier;

  bool              connected_flag{false};
  manual_event_flag rx_path_disconnected;

  /// Identifier of the current E1 TNL association attempt. Only accessed from the CU-UP executor.
  unsigned e1_session_epoch{0};
};

} // namespace ocudu::ocuup
