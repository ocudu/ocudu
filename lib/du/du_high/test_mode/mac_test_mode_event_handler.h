// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/ran/du_types.h"
#include <vector>

namespace ocudu {

class mac_test_mode_event_handler
{
  static constexpr unsigned queue_size = 128U;

public:
  using task_type = unique_function<void(), default_unique_task_buffer_size, true>;

  mac_test_mode_event_handler(unsigned nof_cells)
  {
    cells.reserve(nof_cells);
    for (unsigned i = 0, e = nof_cells; i < e; ++i) {
      cells.emplace_back(std::make_unique<cell_event_queue>(queue_size));
    }
  }

  bool schedule(du_cell_index_t cell_index, task_type t)
  {
    ocudu_assert(cell_index < cells.size(), "Invalid cell index {}", cell_index);
    return cells[cell_index]->try_push(std::move(t));
  }

  void process_pending_tasks(du_cell_index_t cell_index)
  {
    ocudu_assert(cell_index < cells.size(), "Invalid cell index {}", cell_index);
    auto& c = *cells[cell_index];

    task_type task;
    while (c.try_pop(task)) {
      task();
    }
  }

private:
  using cell_event_queue = concurrent_queue<task_type, concurrent_queue_policy::lockfree_mpmc>;

  std::vector<std::unique_ptr<cell_event_queue>> cells;
};

} // namespace ocudu
