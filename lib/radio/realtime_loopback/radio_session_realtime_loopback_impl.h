// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_stream_realtime_loopback_impl.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/synchronization/stop_event.h"
#include <memory>
#include <vector>

namespace ocudu {

/// \brief Realtime loopback radio session.
///
/// Each stream loops its transmitted samples back to its receiver. All the streams share the same RF clock.
class radio_session_realtime_loopback_impl : public radio_session, public radio_capabilities_registry
{
public:
  /// Forbid default constructor.
  radio_session_realtime_loopback_impl() = delete;

  /// Constructor that uses the default realtime emulation based on the system clock.
  radio_session_realtime_loopback_impl(const radio_configuration::radio& config,
                                       task_executor&                    async_task_executor,
                                       radio_event_notifier&             notification_handler);

  /// Constructor that overrides the default realtime emulation with a custom function.
  radio_session_realtime_loopback_impl(const radio_configuration::radio&                    config,
                                       task_executor&                                       async_task_executor,
                                       radio_event_notifier&                                notification_handler,
                                       const unique_function<baseband_gateway_timestamp()>& current_rf_timestamp_fn);

  // See the radio_session interface for documentation.
  radio_capabilities_registry& get_capabilities_registry() override { return *this; }

  // See the radio_session interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    ocudu_assert(stream_id < streams.size(),
                 "Stream identifier (i.e., {}) exceeds the number of streams (i.e., {}).",
                 stream_id,
                 streams.size());
    return *streams[stream_id];
  }

  // See the radio_session interface for documentation.
  baseband_gateway_timestamp read_current_time() override;

  // See the radio_session interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See the radio_session interface for documentation.
  void stop() override;

private:
  // See the radio_capabilities_registry interface for documentation.
  void* query(radio_capability_key key) override { return nullptr; }

  /// The system time corresponding to timestamp 0 in nanoseconds.
  std::chrono::nanoseconds ts0_epoch;

  /// Sampling rate common to all channels.
  double sampling_rate_hz;

  /// Scales RF time relative to the system clock. Values below 1 slow RF time down.
  double rf_time_scale;

  /// Start request flag.
  std::atomic<bool> start_requested;

  /// Stop control.
  rt_stop_event_source stop_control;

  /// \brief Derives the current RF timestamp, based on the system time and the epoch of timestamp 0.
  ///
  /// This is the timestamp of the samples currently at the antenna port, and is common for the transmitter and receiver
  /// chains of all the streams.
  unique_function<baseband_gateway_timestamp()> get_current_rf_timestamp_realtime_clock = [this]() {
    // Get the time since the epoch.
    auto time_since_epoch =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());

    return static_cast<baseband_gateway_timestamp>(static_cast<double>(time_since_epoch.count() - ts0_epoch.count()) *
                                                   sampling_rate_hz * rf_time_scale / 1e9);
  };

  /// \brief Function to be used to compute the current timestamp.
  ///
  /// It can be used to override the default realtime emulation function with deterministic clocks for testing purposes.
  const unique_function<baseband_gateway_timestamp()>& get_current_rf_timestamp;

  /// Loopback streams.
  std::vector<std::unique_ptr<radio_stream_realtime_loopback_impl>> streams;
};

} // namespace ocudu
