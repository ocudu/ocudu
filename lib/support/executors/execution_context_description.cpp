// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/executors/execution_context_description.h"
#include "execution_context_description_setup.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

thread_local unsigned nof_concurrent_workers = 1;
thread_local unsigned worker_index           = 0;

unsigned ocudu::execution_context::get_current_nof_concurrent_workers()
{
  return nof_concurrent_workers;
}

unsigned ocudu::execution_context::get_current_worker_index()
{
  return worker_index;
}

void ocudu::execution_context::set_execution_context_description(unsigned nof_concurrent_workers_,
                                                                 unsigned worker_index_)
{
  ocudu_sanity_check(nof_concurrent_workers_ > 0, "Invalid number of workers");
  nof_concurrent_workers = nof_concurrent_workers_;
  worker_index           = worker_index_;
}
