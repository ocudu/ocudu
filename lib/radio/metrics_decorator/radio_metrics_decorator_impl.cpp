// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_metrics_decorator_impl.h"
#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/dot_prod.h"
#include "ocudu/radio/radio_baseband_metrics.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/math/stats.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"
#include <cmath>
#include <limits>

using namespace ocudu;

namespace {

/// Internal metrics obtained on each radio call.
struct radio_call_metrics {
  /// Number of complex samples per channel passed to or received from the radio.
  unsigned nof_samples_per_channel;
  /// Elapsed time during the driver call and utilized CPU resources.
  resource_usage_utils::measurements measurements;
};

/// Internal IQ metrics obtained on each radio call.
struct radio_iq_metrics {
  /// Linear average power of the signal.
  float avg_power;
  /// Linear peak power of the signal.
  float peak_power;
  /// Clipping counters.
  clipping_counters clipping;
};

} // namespace

/// Computes IQ metrics from any buffer type that provides get_nof_channels() and const get_channel_buffer().
/// Works with baseband_gateway_buffer_reader (TX path) and baseband_gateway_buffer_writer (RX path).
template <typename Buffer>
static radio_iq_metrics compute_iq_metrics(Buffer& data)
{
  constexpr float norm_scale = ocuduvec::scaling_factor_ci16_to_cf;

  sample_statistics<float> peak_power;
  sample_statistics<float> avg_power;
  uint64_t                 nof_clipped_samples     = 0;
  uint64_t                 total_processed_samples = 0;

  for (unsigned i_channel = 0, nof_channels = data.get_nof_channels(); i_channel != nof_channels; ++i_channel) {
    // Extract channel.
    span<const ci16_t> channel_data = data.get_channel_buffer(i_channel);

    // Skip if empty.
    if (channel_data.empty()) {
      continue;
    }

    // Compute IQ metrics.
    float    avg         = ocuduvec::average_power(channel_data, norm_scale);
    auto     max_abs     = ocuduvec::max_abs_element(channel_data, norm_scale);
    uint64_t nof_clipped = ocuduvec::count_if_part_abs_greater_than(channel_data, 0.95f, norm_scale);

    // Combine metrics if the measurements are valid.
    if (std::isnormal(max_abs.second) && std::isnormal(avg)) {
      avg_power.update(avg);
      peak_power.update(max_abs.second);
      nof_clipped_samples += nof_clipped;
      total_processed_samples += channel_data.size();
    }
  }

  return {.avg_power  = avg_power.get_mean(),
          .peak_power = peak_power.get_max(),
          .clipping   = {.nof_clipped_samples = nof_clipped_samples, .nof_processed_samples = total_processed_samples}};
}

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

  // Update receive statistics.
  update_stats(radio_stats.rx_stats, metrics);

  // Notify IQ measurements.
  if (notifier != nullptr) {
    // Compute RX IQ metrics from the received buffer data.
    radio_iq_metrics iq_metrics = compute_iq_metrics(data);

    if (std::isnormal(iq_metrics.avg_power) && std::isnormal(iq_metrics.peak_power)) {
      notifier->on_new_receive_metrics(
          {.avg_power        = iq_metrics.avg_power,
           .peak_power       = iq_metrics.peak_power,
           .clipping         = iq_metrics.clipping,
           .call_duration_ns = static_cast<uint64_t>(metrics.measurements.duration.count())});
    }
  }

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

  // Update transmit statistics.
  update_stats(radio_stats.tx_stats, metrics);

  // Notify measurements.
  if (notifier != nullptr) {
    // Compute TX IQ metrics from the transmitted buffer data.
    radio_iq_metrics iq_metrics = compute_iq_metrics(data);

    if (std::isnormal(iq_metrics.avg_power) && std::isnormal(iq_metrics.peak_power)) {
      notifier->on_new_transmit_metrics(
          {.avg_power        = iq_metrics.avg_power,
           .peak_power       = iq_metrics.peak_power,
           .clipping         = iq_metrics.clipping,
           .call_duration_ns = static_cast<uint64_t>(metrics.measurements.duration.count())});
    }
  }
}

radio_metrics_decorator::radio_metrics_decorator(std::unique_ptr<radio_session>         radio_session_base_,
                                                 span<radio_baseband_metrics_notifier*> notifiers,
                                                 ocudulog::basic_levels                 rf_log_level) :
  radio_session_base(std::move(radio_session_base_)), logger(ocudulog::fetch_basic_logger("RF", false))
{
  report_fatal_error_if_not(radio_session_base != nullptr, "Invalid radio session.");

  logger.set_level(rf_log_level);

  // Create decorators for each stream.
  for (unsigned i_stream = 0, nof_streams = notifiers.size(); i_stream != nof_streams; ++i_stream) {
    decorated_baseband_gateways.emplace_back(radio_session_base->get_baseband_gateway(i_stream), notifiers[i_stream]);
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
