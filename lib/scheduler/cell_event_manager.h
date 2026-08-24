// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include <memory>

namespace ocudu {

class cell_configuration;
class cell_event_dispatcher;
class paging_scheduler;
struct sched_paging_information;

/// \brief Handler of the events of a cell that require no access to the UE repository.
///
/// Events are enqueued from any executor and processed at the start of the cell slot indication, so that their
/// handling runs in the cell scheduler executor.
class cell_event_manager
{
public:
  cell_event_manager(const cell_configuration& cell_cfg, paging_scheduler& pg_sch, ocudulog::basic_logger& logger);
  ~cell_event_manager();

  /// Activate event processing.
  void start();

  /// Deactivate event processing and discard any pending event.
  void stop();

  /// Process the events pending for this cell.
  void run_slot();

  /// Enqueue paging information reported by upper layers.
  void handle_paging_information(const sched_paging_information& pi);

private:
  paging_scheduler& pg_sch;

  // Queue of pending events and pools of the event payloads that do not fit in an event callback.
  std::unique_ptr<cell_event_dispatcher> dispatcher;
};

} // namespace ocudu
