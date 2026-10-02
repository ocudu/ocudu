// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <chrono>
#include <string>
#include <utility>

namespace ocudu {

class timer_manager;
class task_executor;

namespace schedtrace {

class tracer_handle;

/// \brief Initialize the scheduler tracing backend.
/// \param dir_path Directory path where snapshot files will be stored.
/// \param flush_period Period at which event trace file writing occurs. Also determines the event queue size.
/// \param timers Timer manager of the application.
/// \param executor Task executor for handling snapshot writing tasks.
/// \return Handle that tears down the backend on destruction.
[[nodiscard]] tracer_handle init_tracer(const std::string&        dir_path,
                                        std::chrono::milliseconds flush_period,
                                        timer_manager&            timers,
                                        task_executor&            executor);

/// \brief Owner of the scheduler tracing backend.
///
/// The backend must be torn down after all cell tracers are destroyed, and while the timers and executor passed to
/// \c init_tracer are still running.
class tracer_handle
{
public:
  tracer_handle() = default;
  tracer_handle(tracer_handle&& other) noexcept : active(std::exchange(other.active, false)) {}
  tracer_handle& operator=(tracer_handle&& other) noexcept
  {
    reset();
    active = std::exchange(other.active, false);
    return *this;
  }
  ~tracer_handle() { reset(); }

  /// Flushes all pending events and tears down the backend, if active.
  void reset();

private:
  friend tracer_handle init_tracer(const std::string&, std::chrono::milliseconds, timer_manager&, task_executor&);

  explicit tracer_handle(bool active_) : active(active_) {}

  bool active = false;
};

} // namespace schedtrace
} // namespace ocudu
