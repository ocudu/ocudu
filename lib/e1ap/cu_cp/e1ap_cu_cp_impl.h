// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "procedures/e1ap_transaction_manager.h"
#include "ue_context/e1ap_cu_cp_ue_context.h"
#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/e1ap/cu_cp/e1ap_configuration.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu::ocucp {

class e1ap_event_manager;

class e1ap_cu_cp_impl final : public e1ap_cu_cp
{
public:
  e1ap_cu_cp_impl(const e1ap_configuration&      e1ap_cfg_,
                  cu_cp_cu_up_index_t            cu_up_index_,
                  e1ap_message_notifier&         e1ap_pdu_notifier_,
                  e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                  e1ap_cu_cp_notifier&           cu_cp_notifier_,
                  timer_manager&                 timers_,
                  task_executor&                 ctrl_exec_,
                  unsigned                       max_nof_supported_ues_);
  ~e1ap_cu_cp_impl();

  async_task<void> stop() override;

  // e1ap_connection_manager functions.
  async_task<void> handle_cu_cp_e1_reset_message(const cu_cp_reset& reset) override;
  void             handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& msg) override;

  // e1ap_bearer_context_manager functions
  async_task<e1ap_bearer_context_setup_response>
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& request) override;
  async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request) override;
  async_task<void> handle_bearer_context_release_command(const e1ap_bearer_context_release_command& command) override;

  // e1ap_message_handler functions
  void handle_message(const e1ap_message& msg) override;
  void handle_connection_loss() override {}

  // e1ap_ue_handler functions
  void cancel_ue_tasks(cu_cp_ue_index_t ue_index) override;
  void update_ue_context(cu_cp_ue_index_t ue_index, cu_cp_ue_index_t old_ue_index) override;

  // e1ap_bearer_context_removal_handler functions
  void remove_bearer_context(cu_cp_ue_index_t ue_index) override;

  // e1ap_statistics_handler functions
  size_t get_nof_ues() const override { return ue_ctxt_list.size(); }

  // e1ap_cu_cp_interface
  e1ap_message_handler&                get_e1ap_message_handler() override { return *this; }
  e1ap_event_handler&                  get_e1ap_event_handler() override { return *this; }
  e1ap_connection_manager&             get_e1ap_connection_manager() override { return *this; }
  e1ap_bearer_context_manager&         get_e1ap_bearer_context_manager() override { return *this; }
  e1ap_ue_handler&                     get_e1ap_ue_handler() override { return *this; }
  e1ap_bearer_context_removal_handler& get_e1ap_bearer_context_removal_handler() override { return *this; }
  e1ap_statistics_handler&             get_e1ap_statistics_handler() override { return *this; }

private:
  /// \brief Decorator of e1ap_message_notifier that logs the transmitted E1AP messages.
  class e1ap_message_notifier_with_logging final : public e1ap_message_notifier
  {
  public:
    e1ap_message_notifier_with_logging(e1ap_cu_cp_impl& parent_, e1ap_message_notifier& notifier_);

    void on_new_message(const e1ap_message& msg) override;

  private:
    e1ap_cu_cp_impl&       parent;
    e1ap_message_notifier& notifier;
  };

  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::e1ap::init_msg_s& msg);

  /// \brief Handle a Bearer Context Release Request as per TS 37.483 section 8.3.5.
  /// \param[in] msg The received Bearer Context Release Request message.
  void handle_bearer_context_release_request(const asn1::e1ap::bearer_context_release_request_s& msg);

  /// \brief Handle a Bearer Context Inactivity notification as per TS 37.483 section 8.3.6.
  /// \param[in] msg The received Bearer Context Inactivity notification message.
  void handle_bearer_context_inactivity_notification(const asn1::e1ap::bearer_context_inactivity_notif_s& msg);

  /// \brief Handle a DL Data Notification as per TS 37.483 section 8.3.7.
  /// \param[in] msg The received DL Data Notification message.
  void handle_dl_data_notification(const asn1::e1ap::dl_data_notif_s& msg);

  /// \brief Notify about the reception of an successful outcome.
  /// \param[in] msg The received successful outcome message.
  void handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome.
  /// \param[in] msg The received unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome);

  /// \brief Log an E1AP Tx/Rx PDU.
  void log_pdu(bool is_rx, const e1ap_message& e1ap_pdu);

  const e1ap_configuration e1ap_cfg;
  cu_cp_cu_up_index_t      cu_up_index;
  ocudulog::basic_logger&  logger;

  // Notifiers and handlers.
  e1ap_message_notifier_with_logging pdu_notifier;
  e1ap_cu_up_processor_notifier&     cu_up_processor_notifier;
  e1ap_cu_cp_notifier&               cu_cp_notifier;
  task_executor&                     ctrl_exec;

  timer_factory timers;

  // Repository of UE Contexts.
  e1ap_ue_context_list ue_ctxt_list;

  e1ap_transaction_manager ev_mng;

  // Store current E1AP transaction id.
  unsigned current_transaction_id = 0;

  // Flag to indicate if E1 Release procedure is in progress.
  bool e1_release_in_progress = false;

  // Flag to indicate if E1AP is stopping.
  bool e1ap_stopping = false;
};

} // namespace ocudu::ocucp
