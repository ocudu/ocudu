// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_processor.h"
#include "cu_up_processor_config.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"

namespace ocudu::ocucp {

/// CU-UP processor implementation.
class cu_up_processor_impl : public cu_up_processor
{
public:
  cu_up_processor_impl(const cu_up_processor_config& cfg_, const cu_up_processor_dependencies& dependencies);

  // See interface for documentation.
  void stop(cu_cp_ue_index_t ue_idx) override;

  // See interface for documentation.
  void handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg) override;

  // See interface for documentation.
  async_task<void> handle_cu_cp_e1_reset_message(const cu_cp_reset& reset) override;

  // See interface for documentation.
  e1ap_cu_cp& get_e1ap_handler() override { return *e1ap; }

  // See interface for documentation.
  cu_cp_cu_up_index_t get_cu_up_index() override { return context.cu_up_index; }

  // See interface for documentation.
  cu_up_processor_context& get_context() override { return context; }

  // See interface for documentation.
  e1ap_message_handler& get_e1ap_message_handler() override { return *e1ap; }

  // See interface for documentation.
  e1ap_bearer_context_manager& get_e1ap_bearer_context_manager() override { return *e1ap; }

  // See interface for documentation.
  e1ap_bearer_context_removal_handler& get_e1ap_bearer_context_removal_handler() override { return *e1ap; }

  // See interface for documentation.
  e1ap_statistics_handler& get_e1ap_statistics_handler() override { return *e1ap; }

  // See interface for documentation.
  void update_ue_index(cu_cp_ue_index_t ue_index, cu_cp_ue_index_t old_ue_index) override;

private:
  class e1ap_cu_up_processor_adapter;

  // E1AP senders.

  /// Create and transmit the GNB-CU-UP E1 Setup response message.
  void send_cu_up_e1_setup_response();

  /// \brief Create and transmit the GNB-CU-UP E1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_cu_up_e1_setup_failure(e1ap_cause_t cause);

  cu_up_processor_config cfg;

  e1ap_message_notifier& e1ap_notifier;
  e1ap_cu_cp_notifier&   cu_cp_notifier;

  cu_up_processor_context context;

  // E1AP to CU-UP processor adapter.
  std::unique_ptr<e1ap_cu_up_processor_notifier> e1ap_ev_notifier;

  // Components.
  std::unique_ptr<e1ap_cu_cp> e1ap;
};

} // namespace ocudu::ocucp
