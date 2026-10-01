// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace execution_context {

/// Set number of workers and worker index of the current execution context.
void set_execution_context_description(unsigned nof_concurrent_workers, unsigned worker_index);

} // namespace execution_context
} // namespace ocudu
