// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_sidekiq_card.h"
#include "ocudu/radio/radio_management_plane.h"
#include "ocudu/radio/radio_session.h"
#include <vector>

namespace ocudu {

/// Radio session based on Sidekiq that implements the management and data plane functions.
class radio_session_sidekiq_impl : public radio_session, private radio_management_plane
{
public:
  /// Returns the current RF timestamp.
  baseband_gateway_timestamp read_current_time() override;

  /// Constructs a radio session based on Sidekiq.
  radio_session_sidekiq_impl(const radio_configuration::radio& radio_config,
                             task_executor&                    async_executor,
                             radio_event_notifier&             notifier_);

  /// \brief Indicates if the radio session was initialized successfully.
  /// \return True if no exception is caught during initialization. Otherwise false.
  bool is_successful() const { return (state.load() != states::UNINITIALIZED); }

  // See the radio_base interface for documentation.
  radio_management_plane& get_management_plane() override { return *this; }

  // See the radio_base interface for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override
  {
    ocudu_assert(stream_id < cards.size(),
                 "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {})",
                 stream_id,
                 cards.size());
    return *cards[stream_id];
  }

  // See the radio_base interface for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See the radio_base interface for documentation.
  void stop() override;

  // See the radio_management_plane interface for documentation.
  bool set_tx_gain(unsigned port_idx, double gain_dB) override { return set_tx_gain_unprotected(port_idx, gain_dB); }

  // See the radio_management_plane interface for documentation.
  bool set_rx_gain(unsigned port_idx, double gain_dB) override { return set_rx_gain_unprotected(port_idx, gain_dB); }

  // See the radio_management_plane interface for documentation.
  bool set_tx_freq(unsigned stream_id, double center_freq_Hz) override { return false; }

  // See the radio_management_plane interface for documentation.
  bool set_rx_freq(unsigned stream_id, double center_freq_Hz) override { return false; }

private:
  /// Enumerates possible Sidekiq session states.
  enum class states { UNINITIALIZED, SUCCESSFUL_INIT, STOP };
  /// Maps ports to card and channel indexes.
  using port_to_card_channel = std::pair<unsigned, unsigned>;
  /// Current radio session state.
  std::atomic<states> state;
  /// Indexes the transmitter port indexes into card and channel index as first and second.
  static_vector<port_to_card_channel, RADIO_MAX_NOF_PORTS> tx_port_map;
  /// Indexes the receiver port indexes into card and channel index as first and second.
  static_vector<port_to_card_channel, RADIO_MAX_NOF_PORTS> rx_port_map;
  /// Sidekiq cards.
  std::vector<std::unique_ptr<radio_sidekiq_card>> cards;
  /// Indicates if the reception streams require start.
  bool stream_start_required = true;
  /// Asynchronous executor.
  task_executor& async_executor;
  /// Sampling rate in Hertz.
  uint64_t srate_Hz;
  /// Event notifier.
  radio_event_notifier& notifier;
  /// RF logger.
  ocudulog::basic_logger& logger;

  /// \brief Sets transmission gain of an RF port.
  /// \param[in] port_idx Port index.
  /// \param[in] gain_dB Gain value in dB.
  /// \return True if the port index and gain value are valid, and no exception is caught. Otherwise false.
  bool set_tx_gain_unprotected(unsigned port_idx, double gain_dB);

  /// \brief Set reception gain of an RF port.
  /// \param[in] port_idx Port index.
  /// \param[in] gain_dB Gain value in dB.
  /// \return True if the port index and gain value are valid, and no exception is caught. Otherwise false.
  bool set_rx_gain_unprotected(unsigned port_idx, double gain_dB);
};
} // namespace ocudu
