// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/cu_cp/cu_cp_e1_handler.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/executors/task_executor.h"
#include <condition_variable>
#include <map>

namespace ocudu {

class async_task_scheduler;

namespace ocucp {

class cu_up_processor_repository;

/// CU-UP connection manager configuration.
struct cu_up_connection_manager_config {
  unsigned max_nof_cu_ups;
};

/// CU-UP connection manager dependencies.
struct cu_up_connection_manager_dependencies {
  cu_up_processor_repository& cu_ups;
  task_executor&              cu_cp_exec;
  async_task_scheduler&       common_task_sched;
  ocudulog::basic_logger&     logger;
};

/// \brief This class is responsible for allocating the resources in the CU-CP required to handle the establishment
/// or drop of E1 GW connections.
///
/// This class acts as a facade, hiding the details associated with the dispatching of E1 GW events to the
/// the CU-CP through the appropriate task executors.
class cu_up_connection_manager : public cu_cp_e1_handler
{
public:
  cu_up_connection_manager(const cu_up_connection_manager_config&       cfg,
                           const cu_up_connection_manager_dependencies& dependencies);

  // See interface for documentation.
  std::unique_ptr<e1ap_message_notifier>
  handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier) override;

  /// Stops the CU-CP connection manager.
  void stop();

  /// Returns the number of CU-UPs.
  size_t nof_cu_ups() const { return cu_up_count.load(std::memory_order_relaxed); }

private:
  class shared_cu_up_connection_context;
  class e1_gw_to_cu_cp_pdu_adapter;

  // Called by the E1 GW when it disconnects its PDU notifier endpoint.
  void handle_e1_gw_connection_closed(cu_cp_cu_up_index_t cu_up_idx);

  const unsigned              max_nof_cu_ups;
  cu_up_processor_repository& cu_ups;
  task_executor&              cu_cp_exec;
  async_task_scheduler&       common_task_sched;
  ocudulog::basic_logger&     logger;

  std::map<cu_cp_cu_up_index_t, std::shared_ptr<shared_cu_up_connection_context>> cu_up_connections;
  std::atomic<unsigned>                                                           cu_up_count{0};

  std::atomic<bool>       stopped{false};
  std::mutex              stop_mutex;
  std::condition_variable stop_cvar;
  bool                    stop_completed = false;
};

} // namespace ocucp
} // namespace ocudu
