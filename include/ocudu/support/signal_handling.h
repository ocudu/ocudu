// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

using ocudu_signal_handler = void (*)(int signal);

/// \brief Registers the specified function to be called when the user interrupts the program execution (eg: via
/// Ctrl+C).
///
/// Passing a null function pointer disables the current installed handler.
void register_interrupt_signal_handler(ocudu_signal_handler handler);

/// \brief Registers the specified function to be called when the application is about to be forcefully shutdown by an
/// alarm signal.
///
/// Passing a null function pointer disables the current installed handler.
void register_cleanup_signal_handler(ocudu_signal_handler handler);

} // namespace ocudu
