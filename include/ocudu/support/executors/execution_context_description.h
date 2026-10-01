// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace execution_context {

/// \brief Get number of concurrent workers in the current execution context.
///
/// This function will return 1 if called from within a single task worker and the number of threads of a thread pool
/// if called from within the same thread pool.
[[nodiscard]] unsigned get_current_nof_concurrent_workers();

/// \brief Get the index of the worker in the current execution context. If the worker is a single task_worker, this
// function will return 0. If the worker belongs to a thread pool, it will return the index of the thread in the pool.
[[nodiscard]] unsigned get_current_worker_index();

} // namespace execution_context
} // namespace ocudu
