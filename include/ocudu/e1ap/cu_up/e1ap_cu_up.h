// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/cu_up/cu_up_types.h"
#include "ocudu/e1ap/common/e1_setup_messages.h"
#include "ocudu/e1ap/common/e1ap_common.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace ocuup {

/// Handle E1AP interface management procedures as defined in TS 38.463 section 8.2.
class e1ap_connection_manager
{
public:
  virtual ~e1ap_connection_manager() = default;

  /// \brief Connect the CU-UP to CU-CP via E1AP interface.
  [[nodiscard]] virtual bool connect_to_cu_cp() = 0;

  /// \brief Initiates the E1 Setup procedure as per TS 38.463, Section 8.2.3.
  /// \param[in] request The E1SetupRequest message to transmit.
  /// \return Returns a cu_up_e1_setup_response struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise. \remark The CU-UP transmits the E1SetupRequest as per TS 38.463
  /// section 8.2.3 and awaits the response. If a E1SetupFailure is received the E1AP will handle the failure.
  virtual async_task<cu_up_e1_setup_response> handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& request) = 0;

  /// \brief Launches the E1 Release procedure as per TS 37.483, Section 8.2.7.2.2.
  virtual async_task<void> handle_cu_up_e1ap_release_request() = 0;
};

/// Handle E1AP control messages
class e1ap_control_message_handler
{
public:
  virtual ~e1ap_control_message_handler() = default;

  /// \brief Initiates the Bearer Context Release procedure as defined in TS 38.463 section 8.3.5.
  /// It does this by sending the Bearer Context Release Request to the CU-CP.
  virtual void handle_bearer_context_release_request_required(cu_up_ue_index_t ue_index) = 0;

  /// \brief Initiates the Bearer Context Inactivity Notification procedure as defined in TS 38.463 section 8.3.6
  /// \param[in] msg The common type bearer context inactivity notification message to convert and forward to the CU-CP.
  virtual void
  handle_bearer_context_inactivity_notification(const e1ap_bearer_context_inactivity_notification& msg) = 0;

  /// \brief Transmits the DL Data notification to the CU-CP, as defined in TS 38.463 section 8.3.7.
  /// This is done so the CU-CP can resume the inactive bearer context.
  virtual void handle_dl_data_notification_required(cu_up_ue_index_t ue_index) = 0;
};

/// \brief Interface to query statistics from the E1AP interface.
class e1ap_statistics_handler
{
public:
  virtual ~e1ap_statistics_handler() = default;

  /// \brief Get the number of UEs registered at the E1AP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Methods used by E1AP to notify the CU-UP manager of received messages.
/// Used by the main E1AP implementation.
class e1ap_cu_up_manager_message_notifier
{
public:
  virtual ~e1ap_cu_up_manager_message_notifier() = default;

  /// \brief Notifies the CU-UP manager to create a UE context.
  /// \param[in] msg The received bearer context setup message.
  /// \return Returns a bearer context response message containing the index of the created UE context.
  virtual e1ap_bearer_context_setup_response
  on_bearer_context_setup_request_received(const e1ap_bearer_context_setup_request& msg) = 0;

  /// \brief Notifies the CU-UP manager to create a UE context.
  /// \param[in] msg The received bearer context modification message.
  /// \return Returns a bearer context response message containing the index of the created UE context.
  virtual async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request_received(const e1ap_bearer_context_modification_request& msg) = 0;

  /// \brief Notifies the CU-UP manager to release a UE context.
  /// \param[in] msg The received bearer context release command.
  virtual async_task<void>
  on_bearer_context_release_command_received(const e1ap_bearer_context_release_command& msg) = 0;

  /// \brief Notifies the CU-UP manager of a full E1 reset. The CU-UP will release all bearer contexts.
  virtual async_task<void> on_e1_reset_received(const e1ap_reset& msg) = 0;

  /// \brief Schedules async task on CU-UP.
  virtual void on_schedule_cu_up_async_task(async_task<void> task) = 0;

  /// \brief Schedules async task on UE.
  virtual void on_schedule_ue_async_task(cu_up_ue_index_t ue_index, async_task<void> task) = 0;
};

/// Methods used by E1AP to notify the CU-UP manager of connection drops.
/// Used by the E1AP connection manager.
class e1ap_cu_up_manager_connection_notifier
{
public:
  virtual ~e1ap_cu_up_manager_connection_notifier() = default;

  virtual void on_connection_loss(cu_up_e1_index_t e1_index) = 0;
};

/// Combined entry point for E1AP handling.
class e1ap_interface : public e1ap_message_handler,
                       public e1ap_control_message_handler,
                       public e1ap_connection_manager,
                       public e1ap_statistics_handler
{
public:
  ~e1ap_interface() override = default;

  virtual cu_up_e1_index_t get_e1_index() const = 0;
};

/// Combined exit point of the E1AP to CU-UP manager.
class e1ap_cu_up_manager_notifier : public e1ap_cu_up_manager_message_notifier,
                                    public e1ap_cu_up_manager_connection_notifier
{
public:
  virtual ~e1ap_cu_up_manager_notifier() = default;
};

} // namespace ocuup
} // namespace ocudu
