// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace odu {

/// General operational state of the DU.
struct du_manager_context {
  /// Whether the DU is in operational mode.
  bool running = false;
  /// Flag used to signal that the DU was commanded to stop.
  /// \note This may serve as a cancellation signal to other long on-going procedures.
  bool stop_command_received = false;
};

} // namespace odu
} // namespace ocudu
