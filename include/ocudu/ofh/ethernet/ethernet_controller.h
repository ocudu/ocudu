// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ether {

class frame_notifier;

/// \brief Ethernet operation control interface.
///
/// Allows starting and stopping the Ethernet receiver.
class receiver_operation_controller
{
public:
  virtual ~receiver_operation_controller() = default;

  /// \brief Starts the Ethernet receiver operation.
  ///
  /// \param[in] notifier Ethernet frames reception notifier.
  /// \note Caller will be blocked until the receiver is fully started.
  virtual void start(frame_notifier& notifier) = 0;

  /// \brief Stops the Ethernet receiver operation.
  /// \note Caller will be blocked until the receiver is fully stopped.
  virtual void stop() = 0;
};

} // namespace ether
} // namespace ocudu
