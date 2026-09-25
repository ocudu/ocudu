// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_metrics_decorator_impl.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

using namespace ocudu;

namespace {

/// Internal metrics obtained on each radio call.
struct radio_call_metrics {
  /// Number of complex samples per channel passed to or received from the radio.
  unsigned nof_samples_per_channel;
  /// Elapsed time during the driver call and utilized CPU resources.
  resource_usage_utils::measurements measurements;
};

} // namespace

// Updates the radio call statistics with the collected metrics.
static void update_stats(baseband_gateway_decorator::radio_call_statistics& stats, const radio_call_metrics& metrics)
{
  stats.nof_samples_per_channel.update(metrics.nof_samples_per_channel);
  stats.call_duration_s.update(static_cast<double>(metrics.measurements.duration.count()) / 1e9);
}

baseband_gateway_receiver::metadata baseband_gateway_decorator::receive(baseband_gateway_buffer_writer& data)
{
  metadata           ret;
  radio_call_metrics metrics;
  metrics.nof_samples_per_channel = data.get_nof_samples();
  {
    resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements,
                                                               resource_usage_utils::rusage_measurement_type::THREAD);
    ret = gateway_base.get_receiver().receive(data);
  }

  // Update the radio RX statistics. Skip the first time that receive is called to avoid the radio start time.
  static bool is_first = true;
  if (OCUDU_UNLIKELY(is_first)) {
    is_first = false;
    return ret;
  }

  update_stats(radio_stats.rx_stats, metrics);

  return ret;
}

void baseband_gateway_decorator::transmit(const baseband_gateway_buffer_reader&        data,
                                          const baseband_gateway_transmitter_metadata& md)
{
  radio_call_metrics metrics;
  metrics.nof_samples_per_channel = data.get_nof_samples();
  {
    resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements,
                                                               resource_usage_utils::rusage_measurement_type::THREAD);
    gateway_base.get_transmitter().transmit(data, md);
  }

  // Update the radio TX statistics. Skip the first time that transmit is called to avoid the radio start time.
  static bool is_first = true;
  if (OCUDU_UNLIKELY(is_first)) {
    is_first = false;
    return;
  }

  update_stats(radio_stats.tx_stats, metrics);
}

radio_metrics_decorator::radio_metrics_decorator(std::unique_ptr<radio_session> radio_session_base_,
                                                 unsigned                       nof_streams,
                                                 ocudulog::basic_levels         rf_log_level) :
  radio_session_base(std::move(radio_session_base_)), logger(ocudulog::fetch_basic_logger("RF", false))
{
  report_fatal_error_if_not(radio_session_base != nullptr, "Invalid radio session.");

  logger.set_level(rf_log_level);

  // Create decorators for each stream.
  for (unsigned i_stream = 0; i_stream != nof_streams; ++i_stream) {
    decorated_baseband_gateways.emplace_back(radio_session_base->get_baseband_gateway(i_stream));
  }
}

baseband_gateway& radio_metrics_decorator::get_baseband_gateway(unsigned stream_id)
{
  ocudu_assert(stream_id < decorated_baseband_gateways.size(),
               "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {})",
               stream_id,
               decorated_baseband_gateways.size());

  return decorated_baseband_gateways[stream_id];
}

void radio_metrics_decorator::stop()
{
  for (unsigned i_stream = 0, nof_streams = decorated_baseband_gateways.size(); i_stream != nof_streams; ++i_stream) {
    log_radio_stats(decorated_baseband_gateways[i_stream].get_tx_rx_statistics(), i_stream);
  }
  radio_session_base->stop();
}

void radio_metrics_decorator::log_radio_stats(const baseband_gateway_decorator::tx_rx_statistics& stats,
                                              unsigned                                            stream_id)
{
  logger.info("stream {} stats:\n"
              "  TX: nof_channel_samples=[avg={} min={} max={}] radio_call_time=[avg={:.1f}us min={:.1f}us "
              "max={:.1f}us std_dev={:.1f}us] nof_calls={}\n"
              "  RX: nof_channel_samples=[avg={} min={} max={}] radio_call_time=[avg={:.1f}us min={:.1f}us "
              "max={:.1f}us std_dev={:.1f}us] nof_calls={}",
              stream_id,
              stats.tx_stats.nof_samples_per_channel.get_mean(),
              stats.tx_stats.nof_samples_per_channel.get_min(),
              stats.tx_stats.nof_samples_per_channel.get_max(),
              stats.tx_stats.call_duration_s.get_mean() * 1e6,
              stats.tx_stats.call_duration_s.get_min() * 1e6,
              stats.tx_stats.call_duration_s.get_max() * 1e6,
              stats.tx_stats.call_duration_s.get_std() * 1e6,
              stats.tx_stats.nof_samples_per_channel.get_nof_observations(),
              stats.rx_stats.nof_samples_per_channel.get_mean(),
              stats.rx_stats.nof_samples_per_channel.get_min(),
              stats.rx_stats.nof_samples_per_channel.get_max(),
              stats.rx_stats.call_duration_s.get_mean() * 1e6,
              stats.rx_stats.call_duration_s.get_min() * 1e6,
              stats.rx_stats.call_duration_s.get_max() * 1e6,
              stats.rx_stats.call_duration_s.get_std() * 1e6,
              stats.rx_stats.nof_samples_per_channel.get_nof_observations());
}
