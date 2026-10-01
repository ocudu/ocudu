// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../procedures/e2ap_ric_control_procedure.h"
#include "../procedures/e2ap_subscription_delete_procedure.h"
#include "../procedures/e2ap_subscription_setup_procedure.h"
#include "e2_connection_handler.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include "ocudu/support/async/async_event_source.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/executors/task_executor.h"
#include <memory>

namespace ocudu {

class e2_event_manager;

/// E2 implementation dependencies.
struct e2_impl_dependencies {
  ocudulog::basic_logger&  logger;
  e2ap_e2agent_notifier&   agent_notifier;
  timer_factory            timers;
  e2_connection_client&    e2_client;
  e2_subscription_manager& subscription_mngr;
  e2sm_manager&            e2sm_mngr;
  task_executor&           task_exec;
};

/// E2 implementation.
class e2_impl final : public e2_interface
{
public:
  explicit e2_impl(const e2_impl_dependencies& dependencies);

  // See interface for documentation.
  void start() override {}

  // See interface for documentation.
  void stop() override { cancel_event.set(false); }

  // See interface for documentation.
  bool handle_e2_tnl_connection_request() override;

  // See interface for documentation.
  async_task<void> handle_e2_node_initiated_removal_request() override;

  // See interface for documentation.
  async_task<void> handle_e2_disconnection_request() override;

  // See interface for documentation.
  async_task<e2_setup_response_message> handle_e2_setup_request(const e2_setup_request_message& request) override;

  // See interface for documentation.
  void handle_connection_loss() override {}

  // See interface for documentation.
  void handle_message(const e2_message& msg) override;

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] outcome The received initiating message.
  void handle_initiating_message(const asn1::e2ap::init_msg_s& outcome);

  /// \brief Notify about the reception of a successful outcome.
  /// \param[in] outcome The received successful outcome message.
  void handle_successful_outcome(const asn1::e2ap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome.
  /// \param[in] outcome The received unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::e2ap::unsuccessful_outcome_s& outcome);

  /// \brief Notify about the reception of a ric subscription request message.
  /// \param[in] msg The received ric subscription request message.
  void handle_ric_subscription_request(const asn1::e2ap::ric_sub_request_s& msg);

  /// \brief Notify about the reception of a ric control request message.
  /// \param[in] msg The received ric control request message.
  /// \return The ric control response message.
  void handle_ric_control_request(const asn1::e2ap::ric_ctrl_request_s msg);

  /// \brief Notify about the reception of a ric subscription delete request message.
  /// \param[in] msg The received ric subscription delete request message.
  void handle_ric_subscription_delete_request(const asn1::e2ap::ric_sub_delete_request_s& msg);

  /// \brief Notify about the reception of a E2 Connection Update message.
  /// \param[in] msg The received E2 Connection Update message.
  void handle_e2_connection_update(const asn1::e2ap::e2conn_upd_s& msg);

  ocudulog::basic_logger&           logger;
  timer_factory                     timers;
  task_executor&                    ctrl_exec;
  async_event_source<bool>          cancel_event;
  e2_subscription_proc&             subscription_proc;
  e2sm_manager&                     e2sm_mngr;
  std::unique_ptr<e2_event_manager> events;
  fifo_async_task_scheduler         async_tasks;

  e2_connection_handler                connection_handler;
  std::unique_ptr<e2_message_notifier> tx_pdu_notifier;
};

} // namespace ocudu
