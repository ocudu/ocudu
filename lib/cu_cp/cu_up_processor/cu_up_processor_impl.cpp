// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_processor_impl.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "ocudu/support/async/async_task_scheduler.h"

using namespace ocudu;
using namespace ocucp;

/// Adapter between E1AP and CU-UP processor.
class cu_up_processor_impl::e1ap_cu_up_processor_adapter : public e1ap_cu_up_processor_notifier
{
public:
  e1ap_cu_up_processor_adapter(cu_up_processor_impl& parent_, async_task_scheduler& common_task_sched_) :
    parent(parent_), common_task_sched(common_task_sched_)
  {
  }

  // See interface for documentation.
  void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) override
  {
    parent.handle_cu_up_e1_setup_request(msg);
  }

  // See interface for documentation.
  bool schedule_async_task(async_task<void> task) override { return common_task_sched.schedule(std::move(task)); }

private:
  cu_up_processor_impl& parent;
  async_task_scheduler& common_task_sched;
};

cu_up_processor_impl::cu_up_processor_impl(const cu_up_processor_config&       cfg_,
                                           const cu_up_processor_dependencies& dependencies) :
  cfg(cfg_),
  e1ap_notifier(dependencies.e1ap_notifier),
  cu_cp_notifier(dependencies.cu_cp_notifier),
  e1ap_ev_notifier(std::make_unique<e1ap_cu_up_processor_adapter>(*this, dependencies.common_task_sched))
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.cu_up_index;

  // Create E1.
  e1ap = create_e1ap(cfg.e1ap,
                     context.cu_up_index,
                     e1ap_notifier,
                     *e1ap_ev_notifier,
                     cu_cp_notifier,
                     dependencies.timers,
                     dependencies.cu_cp_executor,
                     cfg.max_nof_ues);
}

void cu_up_processor_impl::stop(cu_cp_ue_index_t ue_idx)
{
  // Cancel E1AP running tasks.
  e1ap->cancel_ue_tasks(ue_idx);
}

void cu_up_processor_impl::handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg)
{
  if (msg.gnb_cu_up_name.has_value()) {
    context.cu_up_name = *msg.gnb_cu_up_name;
  }
  context.id = msg.gnb_cu_up_id;

  // TODO: handle response

  // send setup response
  send_cu_up_e1_setup_response();
}

/// Sender for F1AP messages
void cu_up_processor_impl::send_cu_up_e1_setup_response()
{
  cu_up_e1_setup_response response{.success                  = true,
                                   .gnb_cu_cp_name           = context.cu_cp_name,
                                   .cause                    = std::nullopt,
                                   .crit_diagnostics         = std::nullopt,
                                   .packed_e1_setup_request  = {},
                                   .packed_e1_setup_response = {}};

  e1ap->handle_cu_up_e1_setup_response(response);
}

void cu_up_processor_impl::send_cu_up_e1_setup_failure(e1ap_cause_t cause)
{
  cu_up_e1_setup_response response{.success                  = false,
                                   .gnb_cu_cp_name           = std::nullopt,
                                   .cause                    = cause,
                                   .crit_diagnostics         = std::nullopt,
                                   .packed_e1_setup_request  = {},
                                   .packed_e1_setup_response = {}};
  e1ap->handle_cu_up_e1_setup_response(response);
}

async_task<void> cu_up_processor_impl::handle_cu_cp_e1_reset_message(const cu_cp_reset& reset)
{
  return e1ap->handle_cu_cp_e1_reset_message(reset);
}

void cu_up_processor_impl::update_ue_index(cu_cp_ue_index_t ue_index, cu_cp_ue_index_t old_ue_index)
{
  e1ap->update_ue_context(ue_index, old_ue_index);
}
