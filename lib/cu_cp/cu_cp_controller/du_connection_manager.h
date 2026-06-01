// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/cu_cp/cu_cp_f1c_handler.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include "ocudu/support/executors/task_executor.h"
#include <condition_variable>
#include <map>

namespace ocudu::ocucp {

class du_processor_repository;
struct du_setup_request;

/// \brief This class is responsible for allocating the resources in the CU-CP required to handle the establishment
/// or drop of F1-C GW connections.
///
/// This class acts as a facade, hiding the details associated with the dispatching of F1-C GW events to the
/// the CU-CP through the appropriate task executors.
class du_connection_manager : public cu_cp_f1c_handler
{
public:
  du_connection_manager(unsigned                 max_nof_dus,
                        du_processor_repository& dus_,
                        task_executor&           cu_cp_exec_,
                        async_task_scheduler&    common_task_sched_);

  std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) override;

  void stop();

private:
  class shared_du_connection_context;
  class f1_gw_to_cu_cp_pdu_adapter;

  // Called by the F1-C GW when it disconnects its PDU notifier endpoint.
  void handle_f1c_gw_connection_closed(cu_cp_du_index_t du_idx);

  const unsigned           max_nof_dus;
  du_processor_repository& dus;
  task_executor&           cu_cp_exec;
  async_task_scheduler&    common_task_sched;
  ocudulog::basic_logger&  logger;

  std::map<cu_cp_du_index_t, std::shared_ptr<shared_du_connection_context>> du_connections;
  std::atomic<unsigned>                                                     du_count{0};

  std::atomic<bool>       stopped{false};
  std::mutex              stop_mutex;
  std::condition_variable stop_cvar;
  bool                    stop_completed = false;
};

} // namespace ocudu::ocucp
