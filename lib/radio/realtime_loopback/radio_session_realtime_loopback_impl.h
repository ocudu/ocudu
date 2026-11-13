// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_stream_realtime_loopback_impl.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/executors/task_executor.h"
#include <memory>
#include <vector>

namespace ocudu {

/// \brief Realtime loopback radio session.
///
/// Each stream loops its transmitted samples back to its receiver. All the streams share the same RF clock.
class radio_session_realtime_loopback_impl : public radio_session, public radio_capabilities_registry
{
public:
  /// \brief Creates a session that uses the realtime clock.
  ///
  /// The device argument \c rf_time_scale=s, with s > 0, scales the RF clock relative to the steady clock.
  radio_session_realtime_loopback_impl(const radio_configuration::radio& config,
                                       task_executor&                    async_task_executor,
                                       radio_event_notifier&             notification_handler);

  /// \brief Creates a session that uses a custom RF clock.
  ///
  /// The clock outlives the session. A custom clock overrides the realtime emulation, for example, with a deterministic
  /// clock for testing purposes.
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

  /// Steady clock time, in nanoseconds, of RF timestamp 0.
  std::chrono::nanoseconds ts0_epoch;
  /// Sampling rate common to all channels.
  double sampling_rate_hz;
  /// Scales RF time relative to the steady clock. Values below 1 slow RF time down.
  double rf_time_scale;
  /// Start request flag.
  std::atomic<bool> start_requested;
  /// Stop control.
  rt_stop_event_source stop_control;
  /// \brief Realtime RF clock, derived from the steady clock and the epoch of RF timestamp 0.
  ///
  /// It returns the timestamp of the samples currently at the antenna port, common to all the streams.
  unique_function<baseband_gateway_timestamp()> realtime_clock = [this]() {
    auto time_since_epoch =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
    return static_cast<baseband_gateway_timestamp>(static_cast<double>(time_since_epoch.count() - ts0_epoch.count()) *
                                                   sampling_rate_hz * rf_time_scale / 1e9);
  };
  /// RF clock in use, either the realtime clock or a custom clock.
  const unique_function<baseband_gateway_timestamp()>& get_current_rf_timestamp;
  /// Loopback streams.
  std::vector<std::unique_ptr<radio_stream_realtime_loopback_impl>> streams;
};

} // namespace ocudu
