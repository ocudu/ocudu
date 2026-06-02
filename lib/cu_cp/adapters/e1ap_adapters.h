// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cu_cp_impl_interface.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"

namespace ocudu::ocucp {

/// Adapter between E1AP and CU-CP
class e1ap_cu_cp_adapter : public e1ap_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_e1ap_event_handler& cu_cp_handler_) { cu_cp_handler = &cu_cp_handler_; }

  bool schedule_async_task(cu_cp_ue_index_t ue_index, async_task<void> task) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_handler->schedule_ue_task(ue_index, std::move(task));
  }

  void on_bearer_context_release_request_received(const cu_cp_bearer_context_release_request& msg) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_bearer_context_release_request(msg);
  }

  void on_bearer_context_inactivity_notification_received(const e1ap_inactivity_notification& msg) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_bearer_context_inactivity_notification(msg);
  }

  void on_dl_data_notification_received(cu_cp_ue_index_t ue_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_dl_data_notification(ue_index);
  }

  async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_handler->handle_ue_context_release(request);
  }

  void on_e1_release_request_received(cu_cp_cu_up_index_t cu_up_index) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "E1AP handler must not be nullptr");
    cu_cp_handler->handle_e1_release_request(cu_up_index);
  }

  async_task<void> on_transaction_info_loss(const ue_transaction_info_loss_event& ev) override
  {
    ocudu_assert(cu_cp_handler != nullptr, "E1AP handler must not be nullptr");
    return cu_cp_handler->handle_transaction_info_loss(ev);
  }

private:
  cu_cp_e1ap_event_handler* cu_cp_handler = nullptr;
};

} // namespace ocudu::ocucp
