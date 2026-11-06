// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_session_realtime_loopback_impl.h"
#include <cmath>
#include <cstdlib>
#include <string_view>

using namespace ocudu;

/// Returns the \c rf_time_scale value of the device arguments, or 1 if it is absent.
static double get_rf_time_scale(const std::string& args)
{
  static constexpr std::string_view key = "rf_time_scale=";

  // The key must start the arguments or follow a comma.
  std::size_t pos = args.find(key);
  while (pos != std::string::npos && pos != 0 && args[pos - 1] != ',') {
    pos = args.find(key, pos + 1);
  }
  if (pos == std::string::npos) {
    return 1.0;
  }

  const std::size_t value_pos = pos + key.size();
  const std::size_t comma     = args.find(',', value_pos);
  const std::string value = args.substr(value_pos, comma == std::string::npos ? std::string::npos : comma - value_pos);

  char*        end   = nullptr;
  const double scale = std::strtod(value.c_str(), &end);
  report_error_if_not(!value.empty() && (end == value.c_str() + value.size()) && std::isfinite(scale) && (scale > 0.0),
                      "Invalid rf_time_scale in device arguments (i.e., {}).",
                      args);
  return scale;
}

radio_session_realtime_loopback_impl::radio_session_realtime_loopback_impl(const radio_configuration::radio& config,
                                                                           task_executor&        async_task_executor,
                                                                           radio_event_notifier& notification_handler) :
  radio_session_realtime_loopback_impl(config, async_task_executor, notification_handler, realtime_clock)
{
}

radio_session_realtime_loopback_impl::radio_session_realtime_loopback_impl(
    const radio_configuration::radio&                    config,
    task_executor&                                       async_task_executor,
    radio_event_notifier&                                notification_handler,
    const unique_function<baseband_gateway_timestamp()>& current_rf_timestamp_fn) :
  ts0_epoch(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())),
  sampling_rate_hz(config.sampling_rate_Hz),
  rf_time_scale(get_rf_time_scale(config.args)),
  start_requested(false),
  get_current_rf_timestamp(current_rf_timestamp_fn)
{
  report_fatal_error_if_not(config.tx_streams.size() == config.rx_streams.size(),
                            "The number of transmit (i.e., {}) and receive (i.e., {}) streams must be equal.",
                            config.tx_streams.size(),
                            config.rx_streams.size());

  radio_stream_realtime_loopback_impl::shared_state state = {.get_current_rf_timestamp = get_current_rf_timestamp,
                                                             .start_requested          = start_requested,
                                                             .stop_control             = stop_control};

  for (unsigned i_stream = 0, nof_streams = config.rx_streams.size(); i_stream != nof_streams; ++i_stream) {
    unsigned nof_tx_channels = config.tx_streams[i_stream].channels.size();
    unsigned nof_rx_channels = config.rx_streams[i_stream].channels.size();
    report_fatal_error_if_not(nof_tx_channels == nof_rx_channels,
                              "Stream {} has different number of transmit (i.e., {}) and receive (i.e., {}) channels.",
                              i_stream,
                              nof_tx_channels,
                              nof_rx_channels);
    streams.emplace_back(
        std::make_unique<radio_stream_realtime_loopback_impl>(nof_rx_channels, sampling_rate_hz, state));
  }
}

baseband_gateway_timestamp radio_session_realtime_loopback_impl::read_current_time()
{
  return get_current_rf_timestamp();
}

void radio_session_realtime_loopback_impl::start(baseband_gateway_timestamp init_time)
{
  // Reset the stop control. This will block if there is an earlier stop request that has not been completed.
  stop_control.reset();

  if (start_requested.load(std::memory_order_acquire)) {
    report_fatal_error("Called start when radio is already running");
  }

  // The streams are prepared before signaling the start, so that the receivers observe their initial state.
  for (auto& stream : streams) {
    stream->start(init_time);
  }

  start_requested.store(true, std::memory_order_release);
}

void radio_session_realtime_loopback_impl::stop()
{
  stop_control.stop();
}
