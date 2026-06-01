// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_processor/du_metrics_handler.h"
#include "../mobility_manager/mobility_manager_impl.h"
#include "../ngap_repository.h"
#include "../ue_manager/ue_metrics_handler.h"
#include "ocudu/cu_cp/cu_cp_metrics_handler.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <deque>
#include <mutex>

namespace ocudu::ocucp {

class metrics_handler_impl final : public metrics_handler
{
public:
  metrics_handler_impl(task_executor&                    cu_cp_exec_,
                       timer_manager&                    timers,
                       ue_metrics_handler&               ue_handler_,
                       du_repository_metrics_handler&    du_handler_,
                       ngap_repository_metrics_handler&  ngap_handler_,
                       mobility_manager_metrics_handler& mobility_handler_);

  std::unique_ptr<metrics_report_session>
  create_periodic_report_session(const periodic_metric_report_request& request) override;

  cu_cp_metrics_report request_metrics_report() const override;

private:
  /// Context of a periodic metric report session
  struct periodic_session_context {
    unique_timer                   timer;
    cu_cp_metrics_report_notifier* report_notifier;
  };

  // Generate new metrics report.
  cu_cp_metrics_report create_report() const;

  unsigned create_periodic_session(const periodic_metric_report_request& request);
  void     request_session_reconfiguration(unsigned session_id, const periodic_metric_report_request& request);
  void     request_session_deletion(unsigned session_id);

  task_executor&                    cu_cp_exec;
  timer_manager&                    timers;
  ue_metrics_handler&               ue_handler;
  du_repository_metrics_handler&    du_handler;
  ngap_repository_metrics_handler&  ngap_handler;
  mobility_manager_metrics_handler& mobility_handler;
  ocudulog::basic_logger&           logger;

  // Member variables to manage pool of sessions.
  std::mutex                           mutex;
  std::deque<periodic_session_context> sessions;
  std::vector<unsigned>                free_list;
};

} // namespace ocudu::ocucp
