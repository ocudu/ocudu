// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <chrono>
#include <string>

namespace ocudu {

class timer_manager;
class task_executor;

namespace schedtrace {

/// \brief Initialize the scheduler tracing backend.
/// \param dir_path Directory path where snapshot files will be stored.
/// \param flush_period Period at which event trace file writing occurs. Also determines the event queue size.
/// \param timers Timer manager of the application.
/// \param executor Task executor for handling snapshot writing tasks.
void init_tracer(const std::string&        dir_path,
                 std::chrono::milliseconds flush_period,
                 timer_manager&            timers,
                 task_executor&            executor);

/// \brief Tear down the scheduler tracing backend.
/// \remark Automatically called at the end of the application. However, for testing we may want to close it earlier.
void close_tracer();

} // namespace schedtrace
} // namespace ocudu
