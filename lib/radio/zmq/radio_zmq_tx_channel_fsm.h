// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_zmq_timer.h"
#include "ocudu/support/ocudu_assert.h"
#include <condition_variable>
#include <mutex>

namespace ocudu {

/// Finite state machine for the ZeroMQ-based radio transmit channel.
class radio_zmq_tx_channel_fsm
{
  /// Describes the possible states.
  enum class states {
    /// Indicates it failed to initialize.
    UNINITIALIZED = 0,
    /// The channel has been successfully initialised and it is waiting to start.
    WAIT_START,
    /// The channel waits for a request.
    WAIT_REQUEST,
    /// The channel waits for data to transmit.
    WAIT_DATA,
    /// Indicates it is notify_stop.
    STOPPED
  };

  /// Indicates the current state.
  states state = states::UNINITIALIZED;
  /// Protects the state and transitions.
  mutable std::mutex mutex;
  /// State waiting timer.
  radio_zmq_timer timer;

  /// Same than is_running().
  bool is_running_unprotected() const { return state == states::WAIT_REQUEST || state == states::WAIT_DATA; }

public:
  /// Notifies that the transmit stream has been initialized successfully.
  void init_successful()
  {
    std::scoped_lock lock(mutex);
    ocudu_assert(state == states::UNINITIALIZED, "Invalid state.");
    state = states::WAIT_START;
  }

  /// Notifies the asynchronous processing has started.
  void on_start()
  {
    std::scoped_lock lock(mutex);
    ocudu_assert(state == states::WAIT_START, "Invalid state.");
    state = states::WAIT_REQUEST;
    timer.start();
  }

  /// Notifies that the stream detected an error it cannot recover from.
  void on_error()
  {
    std::scoped_lock lock(mutex);
    state = states::STOPPED;
  }

  /// Notifies that a request was received.
  void request_received()
  {
    std::scoped_lock lock(mutex);
    if (state == states::WAIT_REQUEST) {
      state = states::WAIT_DATA;
      timer.start();
    }
  }

  /// Notifies a block of data was send.
  void data_sent()
  {
    std::scoped_lock lock(mutex);
    if (state == states::WAIT_DATA) {
      state = states::WAIT_REQUEST;
      timer.start();
    }
  }

  /// Notifies a stop was signaled and wait for asynchronous executor to stop.
  void stop()
  {
    std::scoped_lock lock(mutex);
    if (is_running_unprotected()) {
      state = states::STOPPED;
    }
  }

  /// Notifies the asynchronous task has notify_stop.
  void async_task_stopped()
  {
    std::scoped_lock lock(mutex);
    state = states::STOPPED;
  }

  /// Gets if the channel has been successfully initialised.
  bool is_initiated() const
  {
    std::scoped_lock lock(mutex);
    return state > states::UNINITIALIZED;
  }

  /// Indicates whether the current state is operational.
  bool is_running() const
  {
    std::scoped_lock lock(mutex);
    return is_running_unprotected();
  }

  /// Indicates whether it has a pending request.
  bool has_pending_request() const
  {
    std::scoped_lock lock(mutex);
    return state == states::WAIT_DATA;
  }

  /// \brief Checks if the waiting time for request or data has expired.
  ///
  /// The request and data wait expires only if:
  /// - the current state is waiting for a request; or
  /// - the current state is waiting for data.
  ///
  /// The expiration occurs when a certain time had past since:
  /// - the last state transition from waiting data to waiting for request; or
  /// - last time an expiration was detected.
  bool has_wait_expired()
  {
    std::scoped_lock lock(mutex);

    // It can only be expired if it is currently running.
    if (!is_running_unprotected()) {
      return false;
    }

    // Check if the timer has not expired.
    if (!timer.is_expired()) {
      return false;
    }

    // Update timer with new time.
    timer.start();
    return true;
  }
};

} // namespace ocudu
