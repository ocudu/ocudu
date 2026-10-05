// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/gateways/baseband/baseband_gateway_receiver.h"
#include "ocudu/gateways/baseband/baseband_gateway_transmitter.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/radio/radio_baseband_metrics_notifier.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/math/stats.h"
#include <memory>

namespace ocudu {

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

  baseband_gateway_decorator() = delete;

  /// Constructor that decorates a baseband gateway passed by reference.
  ///
  /// \param gateway_base_  Base gateway to decorate.
  /// \param notifier_      Optional baseband metrics notifier (nullptr = no notification or IQ metrics).
  baseband_gateway_decorator(baseband_gateway& gateway_base_, radio_baseband_metrics_notifier* notifier_ = nullptr) :
    gateway_base(gateway_base_), notifier(notifier_)
  {
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
  metadata receive(baseband_gateway_buffer_writer& data) override;

  // See the baseband_gateway_transmitter interface for documentation.
  void transmit(const baseband_gateway_buffer_reader& data, const baseband_gateway_transmitter_metadata& md) override;

  /// Gets the collected transmit and receive statistics.
  const tx_rx_statistics& get_tx_rx_statistics() const { return radio_stats; }

private:
  /// Base gateway to be decorated.
  baseband_gateway& gateway_base;
  /// Optional radio baseband metric notifier (nullptr = no notification or IQ metrics).
  radio_baseband_metrics_notifier* notifier;
  /// Transmit and receive radio statistics.
  tx_rx_statistics radio_stats;
};

/// Decorator for a radio session that adds transmit and receive call metrics.
class radio_metrics_decorator : public radio_session
{
public:
  /// Forbid default constructor.
  radio_metrics_decorator() = delete;

  /// \brief Constructor that takes ownership of a base radio and provides decorated transmit and receive calls.
  ///
  /// \param radio_session_base_  Base radio session to decorate.
  /// \param notifiers            Optional radio baseband metric notifiers, one per stream (empty or nullptr = no
  ///                             notification or IQ metrics).
  /// \param rf_log_level         RF log level to use in the decorator.
  radio_metrics_decorator(std::unique_ptr<radio_session>         radio_session_base_,
                          span<radio_baseband_metrics_notifier*> notifiers,
                          ocudulog::basic_levels                 rf_log_level);

  // See the radio_session interface for documentation.
  radio_management_plane& get_management_plane() override { return radio_session_base->get_management_plane(); }

  // See the radio_session interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override;

  // See the radio_session interface for documentation.
  baseband_gateway_timestamp read_current_time() override { return radio_session_base->read_current_time(); }

  // See the radio_session interface for documentation.
  void start(baseband_gateway_timestamp init_time) override { radio_session_base->start(init_time); }

  // See the radio_session interface for documentation.
  void stop() override;

private:
  /// Logs the collected radio statistics for a stream.
  void log_radio_stats(const baseband_gateway_decorator::tx_rx_statistics& stats, unsigned stream_id);

  /// Base radio session instance.
  std::unique_ptr<radio_session> radio_session_base;

  /// Radio session logger.
  ocudulog::basic_logger& logger;

  /// List of internal baseband gateway decorators.
  static_vector<baseband_gateway_decorator, RADIO_MAX_NOF_STREAMS> decorated_baseband_gateways;
};

} // namespace ocudu
