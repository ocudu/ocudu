// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/task_executor.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>

namespace ocudu::ocucp {

/// \brief Run the given callable on the executor and wait (bounded, cancelled on timeout) for its result.
///
/// Callers running outside the CU-CP execution context (e.g. WS/O1 handlers on the IO broker thread) use this
/// to marshal work onto the CU-CP executor, blocking only until the synchronous result is known: every CU-CP
/// structure is then touched exclusively from the CU-CP execution context.
///
/// The wait is bounded and the state is shared with the queued task: if the executor is saturated or stops
/// before running the task, the dispatch fails instead of hanging the caller's thread. A timed-out dispatch
/// is also cancelled — the queued task checks the flag and becomes a no-op — so the caller's failure report
/// stays truthful: the callable does not run behind its back, unless the task had already started when the
/// timeout expired.
///
/// \return The callable's result, or std::nullopt when the executor rejected the task or the wait timed out.
template <typename T>
std::optional<T>
dispatch_bounded(task_executor& executor, ocudulog::basic_logger& logger, const char* name, std::function<T()> fn)
{
  static constexpr std::chrono::seconds dispatch_timeout{5};

  auto           result_promise = std::make_shared<std::promise<T>>();
  auto           cancelled      = std::make_shared<std::atomic<bool>>(false);
  std::future<T> fut            = result_promise->get_future();

  if (not executor.execute([result_promise, cancelled, fn = std::move(fn)]() {
        if (cancelled->load(std::memory_order_acquire)) {
          return;
        }
        result_promise->set_value(fn());
      })) {
    logger.warning("Dispatch {} failed. Cause: CU-CP executor queue is full", name);
    return std::nullopt;
  }
  if (fut.wait_for(dispatch_timeout) != std::future_status::ready) {
    cancelled->store(true, std::memory_order_release);
    logger.warning("Dispatch {} timed out after {}s. The command was cancelled and does not run, unless it had "
                   "already started",
                   name,
                   dispatch_timeout.count());
    return std::nullopt;
  }
  return fut.get();
}

} // namespace ocudu::ocucp
