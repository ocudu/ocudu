// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_processor.h"
#include "ocudu/e1ap/cu_cp/e1ap_configuration.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/timers.h"
#include <map>

namespace ocudu {

class async_task_scheduler;

namespace ocucp {

/// CU-UP repository configuration.
struct cu_up_repository_config {
  e1ap_configuration e1ap;
  unsigned           max_nof_cu_ups;
  uint32_t           max_nof_ues;
};

/// CU-UP repository dependencies.
struct cu_up_repository_dependencies {
  task_executor&          cu_cp_executor;
  timer_manager&          timers;
  e1ap_cu_cp_notifier&    e1ap_ev_notifier;
  async_task_scheduler&   common_task_sched;
  ocudulog::basic_logger& logger;
};

class cu_up_processor_repository
{
public:
  cu_up_processor_repository(const cu_up_repository_config& cfg_, const cu_up_repository_dependencies& dependencies);

  /// \brief Adds a CU-UP processor object to the CU-CP.
  /// \return The CU-UP index of the added CU-UP processor object.
  cu_cp_cu_up_index_t add_cu_up(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier);

  /// \brief Removes the specified CU-UP processor object from the CU-CP.
  /// \param[in] cu_up_index The index of the CU-UP processor to delete.
  async_task<void> remove_cu_up(cu_cp_cu_up_index_t cu_up_index);

  /// Gets the number of CU-UPs currently connected.
  size_t get_nof_cu_ups() const { return cu_up_db.size(); }

  /// Gets the CU-CP processor E1AP interface for the given CU-CP index.
  cu_up_processor_e1ap_interface& get_cu_up(cu_cp_cu_up_index_t cu_up_index);

  /// \brief Find a CU-UP object.
  /// \param[in] cu_up_index The index of the CU-UP processor object.
  /// \return The CU-UP processor object if it exists, nullptr otherwise.
  cu_up_processor* find_cu_up_processor(cu_cp_cu_up_index_t cu_up_index);

  /// \brief Select a CU-UP.
  /// \return The CU-UP index of the selected CU-UP.
  cu_cp_cu_up_index_t select_cu_up();

  /// Gets the number of E1AP UEs.
  size_t get_nof_e1ap_ues() const;

private:
  /// CU-UP context.
  struct cu_up_context {
    /// CU-UP processor.
    std::unique_ptr<cu_up_processor> processor;
    /// Notifier used by the CU-CP to push E1AP Tx messages to the respective CU-UP.
    std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier;
  };

  /// \brief Get the next available index from the CU-UP processor database.
  /// \return The CU-UP index.
  cu_cp_cu_up_index_t allocate_cu_up_index();

  cu_up_repository_config cfg;

  task_executor&          cu_cp_executor;
  timer_manager&          timers;
  e1ap_cu_cp_notifier&    e1ap_ev_notifier;
  async_task_scheduler&   common_task_sched;
  ocudulog::basic_logger& logger;

  std::map<cu_cp_cu_up_index_t, cu_up_context> cu_up_db;
};

} // namespace ocucp
} // namespace ocudu
