// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_manager.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up.h"
#include "ocudu/e1ap/gateways/e1_connection_client.h"
#include "ocudu/support/async/manual_event.h"

namespace ocudu {
namespace ocuup {

class e1ap_cu_up_connection_handler
{
public:
  e1ap_cu_up_connection_handler(cu_up_e1_index_t                        e1_index_,
                                e1_connection_client&                   e1ap_client_handler_,
                                e1ap_message_handler&                   e1ap_pdu_handler_,
                                e1ap_cu_up_manager_connection_notifier& cu_up_manager_,
                                task_executor&                          cu_up_executor_);
  ~e1ap_cu_up_connection_handler();

  [[nodiscard]] e1ap_message_notifier* connect_to_cu_cp();
  [[nodiscard]] bool                   is_connected() const { return connected_flag; }

  async_task<void> handle_tnl_association_removal();

private:
  void handle_connection_loss();
  void handle_connection_loss_impl();

  cu_up_e1_index_t e1_index;

  e1_connection_client&                   e1_client_handler;
  e1ap_message_handler&                   e1ap_pdu_handler;
  e1ap_cu_up_manager_connection_notifier& cu_up_manager;
  task_executor&                          cu_up_executor;
  ocudulog::basic_logger&                 logger;

  std::unique_ptr<e1ap_message_notifier> e1ap_notifier;

  bool              connected_flag{false};
  manual_event_flag rx_path_disconnected;
};

} // namespace ocuup
} // namespace ocudu
