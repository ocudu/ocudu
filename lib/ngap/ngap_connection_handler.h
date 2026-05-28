// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/ngap.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/manual_event.h"

namespace ocudu {

class task_executor;

namespace ocucp {

class n2_connection_client;

class ngap_connection_handler
{
public:
  ngap_connection_handler(cu_cp_amf_index_t     amf_index_,
                          n2_connection_client& client_handler_,
                          ngap_message_handler& rx_pdu_handler_,
                          ngap_cu_cp_notifier&  cu_cp_notifier_,
                          task_executor&        ctrl_exec_);
  ~ngap_connection_handler();

  /// Initiate new TNL association to the AMF via N2 interface.
  /// \return Notifier used by the NGAP to send NGAP PDUs to the N2 gateway.
  std::unique_ptr<ngap_message_notifier> connect_to_amf();

  async_task<void> handle_tnl_association_removal();

  /// \brief Check if the connection is active.
  [[nodiscard]] bool is_connected() const { return connected_flag; }

private:
  // Called by the N2 GW when the N2 TNL association drops.
  void handle_connection_loss();

  // Called from within NGAP execution context to handle a TNL association loss.
  void handle_connection_loss_impl();

  cu_cp_amf_index_t       amf_index;
  n2_connection_client&   client_handler;
  ngap_message_handler&   rx_pdu_handler;
  ngap_cu_cp_notifier&    cu_cp_notifier;
  task_executor&          ctrl_exec;
  ocudulog::basic_logger& logger;

  bool connected_flag{false};

  std::unique_ptr<ngap_message_notifier> tx_pdu_notifier;

  manual_event_flag rx_path_disconnected;
};

} // namespace ocucp
} // namespace ocudu
