// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_factory.h"
#include "ocudu/support/math/stats.h"
#include "ocudu/support/resource_usage/scoped_resource_usage.h"

using namespace ocudu;

namespace {

/// Decorator for the baseband gateway and its associated transmit and receive methods.
class baseband_gateway_decorator : public baseband_gateway,
                                   public baseband_gateway_transmitter,
                                   public baseband_gateway_receiver
{
public:
  /// Baseband gateway call execution statistics.
  struct radio_call_statistics {
    /// Transmit driver call time stats.
    sample_statistics<double> call_duration_s;
    /// Number of samples per channel passed to or received from the radio.
    sample_statistics<unsigned> nof_samples_per_channel;
  };

  /// Baseband gateway transmit and receive call execution statistics.
  struct tx_rx_statistics {
    /// Transmit statistics.
    radio_call_statistics tx_stats;
    /// Receive statistics.
    radio_call_statistics rx_stats;
  };

  /// Constructor that decorates a baseband gateway passed by reference.
  baseband_gateway_decorator(baseband_gateway& gateway_base_) : gateway_base(gateway_base_)
  {
    radio_stats.tx_stats.nof_samples_per_channel.reset();
    radio_stats.rx_stats.nof_samples_per_channel.reset();
    radio_stats.tx_stats.call_duration_s.reset();
    radio_stats.rx_stats.call_duration_s.reset();
  }

  // See the baseband_gateway interface for documentation.
  unsigned get_transmitter_optimal_buffer_size() const override
  {
    return gateway_base.get_transmitter_optimal_buffer_size();
  }

  // See the baseband_gateway interface for documentation.
  unsigned get_receiver_optimal_buffer_size() const override { return gateway_base.get_receiver_optimal_buffer_size(); }

  // See the baseband_gateway interface for documentation.
  baseband_gateway_transmitter& get_transmitter() override { return *this; }

  // See the baseband_gateway interface for documentation.
  baseband_gateway_receiver& get_receiver() override { return *this; }

  // See the baseband_gateway_receiver interface for documentation.
  metadata receive(baseband_gateway_buffer_writer& data) override
  {
    metadata           ret;
    radio_call_metrics metrics;
    metrics.nof_samples_per_channel = data.get_nof_samples();
    {
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements,
                                                                 resource_usage_utils::rusage_measurement_type::THREAD);
      ret = gateway_base.get_receiver().receive(data);
    }

    // Update the radio RX statistics.
    update_stats(radio_stats.rx_stats, metrics);

    return ret;
  }

  // See the baseband_gateway_transmitter interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter_metadata& md) override
  {
    radio_call_metrics metrics;
    metrics.nof_samples_per_channel = data.get_nof_samples();
    {
      resource_usage_utils::scoped_resource_usage rusage_tracker(metrics.measurements,
                                                                 resource_usage_utils::rusage_measurement_type::THREAD);
      gateway_base.get_transmitter().transmit(data, md);
    }

    // Update the radio TX statistics.
    update_stats(radio_stats.tx_stats, metrics);
  }

  /// Gets the collected transmit and receive statistics.
  const tx_rx_statistics& get_tx_rx_statistics() const { return radio_stats; }

private:
  /// Internal metrics obtained on each radio call.
  struct radio_call_metrics {
    /// Number of complex samples per channel passed to or received from the radio.
    unsigned nof_samples_per_channel;
    /// Elapsed time during the driver call and utilized CPU resources.
    resource_usage_utils::measurements measurements;
  };

  /// Updates the radio call statistics.
  static void update_stats(radio_call_statistics& stats, const radio_call_metrics& metrics)
  {
    stats.nof_samples_per_channel.update(metrics.nof_samples_per_channel);
    stats.call_duration_s.update(static_cast<double>(metrics.measurements.duration.count()) / 1e9);
  }

  /// Base gateway to be decorated.
  baseband_gateway& gateway_base;
  /// Transmit and receive radio statistics.
  tx_rx_statistics radio_stats;
};

/// Decorator for a radio session.
class radio_session_decorator : public radio_session
{
public:
  /// Forbid default constructor.
  radio_session_decorator() = delete;

  /// \brief Constructor that takes ownership of a base radio and provides decorated transmit and receive calls.
  ///
  /// \param[in] radio_session_base_ Base radio session to decorate.
  /// \param[in] nof_streams         Number of configured streams in the base radio session.
  /// \param[in] rf_log_level        RF log level to use in the decorator.
  radio_session_decorator(std::unique_ptr<radio_session> radio_session_base_,
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

  // See the radio_session interface for documentation.
  radio_management_plane& get_management_plane() override { return radio_session_base->get_management_plane(); }

  // See the radio_session interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    ocudu_assert(stream_id < decorated_baseband_gateways.size(),
                 "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {})",
                 stream_id,
                 decorated_baseband_gateways.size());

    return decorated_baseband_gateways[stream_id];
  }

  // See the radio_session interface for documentation.
  baseband_gateway_timestamp read_current_time() override { return radio_session_base->read_current_time(); }

  // See the radio_session interface for documentation.
  void start(baseband_gateway_timestamp init_time) override { radio_session_base->start(init_time); }

  // See the radio_session interface for documentation.
  void stop() override
  {
    for (unsigned i_stream = 0, nof_streams = decorated_baseband_gateways.size(); i_stream != nof_streams; ++i_stream) {
      log_radio_stats(decorated_baseband_gateways[i_stream].get_tx_rx_statistics(), i_stream);
    }
    radio_session_base->stop();
  }

private:
  /// Logs the collected radio statistics.
  void log_radio_stats(const baseband_gateway_decorator::tx_rx_statistics& stats, unsigned stream_id)
  {
    logger.info("stream {} stats:\n"
                "  TX: nof_channel_samples=[avg={} min={} max={}] radio_call_time=[avg={:.1f}us min={:.1f}us "
                "max={:.1f}us std_dev={:.4f}] nof_calls={}\n"
                "  RX: nof_channel_samples=[avg={} min={} max={}] radio_call_time=[avg={:.1f}us min={:.1f}us "
                "max={:.1f}us std_dev={:.4f}] nof_calls={}",
                stream_id,
                stats.tx_stats.nof_samples_per_channel.get_mean(),
                stats.tx_stats.nof_samples_per_channel.get_min(),
                stats.tx_stats.nof_samples_per_channel.get_max(),
                stats.tx_stats.call_duration_s.get_mean() * 1e6,
                stats.tx_stats.call_duration_s.get_min() * 1e6,
                stats.tx_stats.call_duration_s.get_max() * 1e6,
                stats.tx_stats.call_duration_s.get_std(),
                stats.tx_stats.nof_samples_per_channel.get_nof_observations(),
                stats.rx_stats.nof_samples_per_channel.get_mean(),
                stats.rx_stats.nof_samples_per_channel.get_min(),
                stats.rx_stats.nof_samples_per_channel.get_max(),
                stats.rx_stats.call_duration_s.get_mean() * 1e6,
                stats.rx_stats.call_duration_s.get_min() * 1e6,
                stats.rx_stats.call_duration_s.get_max() * 1e6,
                stats.rx_stats.call_duration_s.get_std(),
                stats.rx_stats.nof_samples_per_channel.get_nof_observations());
  }

  /// Base radio session instance.
  std::unique_ptr<radio_session> radio_session_base;

  /// Radio session logger.
  ocudulog::basic_logger& logger;

  /// List of internal baseband gateway decorators.
  static_vector<baseband_gateway_decorator, RADIO_MAX_NOF_STREAMS> decorated_baseband_gateways;
};

/// Radio factory that creates decorated radio sessions.
class radio_decorator_factory : public radio_factory
{
public:
  /// \brief Constructor that takes ownership of a base radio radio factory, used to create decorated radio sessions.
  ///
  /// \param[in] radio_factory_base_ Base radio factory to create radio instances.
  /// \param[in] rf_log_level_       RF log level used by the decorator logger.
  radio_decorator_factory(std::unique_ptr<radio_factory> radio_factory_base_, ocudulog::basic_levels rf_log_level_) :
    radio_factory_base(std::move(radio_factory_base_)), rf_log_level(rf_log_level_)
  {
    report_fatal_error_if_not(radio_factory_base != nullptr, "Invalid base radio factory.");
  }

  // See interface for documentation.
  const radio_configuration::validator& get_configuration_validator() const override
  {
    return radio_factory_base->get_configuration_validator();
  }

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_event_notifier&             notifier) override
  {
    std::unique_ptr<radio_session> radio_session_base =
        radio_factory_base->create(config, async_task_executor, notifier);
    return std::make_unique<radio_session_decorator>(
        std::move(radio_session_base), config.tx_streams.size(), rf_log_level);
  }

private:
  /// Base radio factory.
  std::unique_ptr<radio_factory> radio_factory_base;
  /// RF log level.
  ocudulog::basic_levels rf_log_level;
};

} // namespace

std::unique_ptr<radio_factory> ocudu::create_radio_decorator_factory(std::unique_ptr<radio_factory> radio_factory_base_,
                                                                     ocudulog::basic_levels         rf_log_level)
{
  return std::make_unique<radio_decorator_factory>(std::move(radio_factory_base_), rf_log_level);
}
