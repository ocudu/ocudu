// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <chrono>

namespace ocudu {

/// Timer for the ZeroMQ-based radio channel.
class radio_zmq_timer
{
public:
  /// \brief Creates a timer.
  /// \param[in] start_now Set to true for starting immediately.
  radio_zmq_timer(bool start_now = false)
  {
    if (start_now) {
      start();
    }
  }

  /// Starts or restarts the expiration time from the current time.
  void start() { wait_expire_time = std::chrono::steady_clock::now() + wait_timeout; }

  /// Clears the current expiration time.
  void clear() { wait_expire_time = no_time; }

  /// Checks if the time has expired.
  bool is_expired() const
  {
    // No expiration time configured.
    if (wait_expire_time == no_time) {
      return false;
    }

    // Check if the timer has been expired.
    if (std::chrono::steady_clock::now() < wait_expire_time) {
      return false;
    }

    return true;
  }

private:
  /// No time value.
  static constexpr std::chrono::time_point<std::chrono::steady_clock> no_time{};
  /// Timeout value.
  static constexpr std::chrono::milliseconds wait_timeout{1000};

  /// Current expiration time.
  std::chrono::time_point<std::chrono::steady_clock> wait_expire_time = no_time;
};

} // namespace ocudu
