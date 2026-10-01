// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_session_sidekiq_impl.h"
#include <ocudu/ocudulog/ocudulog.h>
#include <sidekiq_api.h>

using namespace ocudu;

static int32_t sidekiq_log_level = SKIQ_LOG_INFO;
static void    rf_sidekiq_log_msg(int32_t priority, const char* message)
{
  if (priority <= sidekiq_log_level) {
    fmt::print(message);
  }
}

radio_session_sidekiq_impl::radio_session_sidekiq_impl(const radio_configuration::radio& radio_config,
                                                       task_executor&                    async_executor_,
                                                       radio_event_notifier&             notifier_) :
  async_executor(async_executor_), notifier(notifier_), logger(ocudulog::fetch_basic_logger("RF"))
{
  if (radio_config.rx_streams.size() > 2 || (radio_config.tx_streams.size() > 2)) {
    fmt::print("Only up to two channels per stream is currently supported.\n");
    return;
  }

  if (radio_config.rx_streams.size() != radio_config.tx_streams.size()) {
    fmt::print("Different number of transmit and receive streams is not supported.\n");
    return;
  }

  for (const radio_configuration::stream& stream_config : radio_config.rx_streams) {
    if (stream_config.channels.size() > 2) {
      fmt::print("Only up to two channels per stream is currently supported.\n");
      return;
    }
  }

  for (const radio_configuration::stream& stream_config : radio_config.tx_streams) {
    if (stream_config.channels.size() > 2) {
      fmt::print("Only up to two channels per stream is currently supported.\n");
      return;
    }
  }

  switch (radio_config.log_level) {
    case ocudulog::basic_levels::debug:
      sidekiq_log_level = SKIQ_LOG_DEBUG;
      break;
    case ocudulog::basic_levels::info:
      sidekiq_log_level = SKIQ_LOG_INFO;
      break;
    case ocudulog::basic_levels::warning:
      sidekiq_log_level = SKIQ_LOG_WARNING;
      break;
    case ocudulog::basic_levels::error:
    case ocudulog::basic_levels::none:
    case ocudulog::basic_levels::LAST:
      sidekiq_log_level = SKIQ_LOG_ERROR;
      break;
  }

  // Register Logger
  skiq_register_logging(&rf_sidekiq_log_msg);

  // Get available cards
  uint8_t nof_available_cards                 = 0;
  uint8_t available_cards[SKIQ_MAX_NUM_CARDS] = {};
  if (skiq_get_cards(skiq_xport_type_auto, &nof_available_cards, available_cards)) {
    fmt::print("Getting available cards\n");
    return;
  }

  if ((nof_available_cards < radio_config.tx_streams.size()) ||
      (nof_available_cards < radio_config.rx_streams.size())) {
    fmt::print("The number of available sidekiq cards, i.e., {} is not sufficient to deploy {} Rx and {} Tx streams\n",
               nof_available_cards,
               radio_config.rx_streams.size(),
               radio_config.rx_streams.size());
    return;
  }

  // Populate the Tx and Rx port maps. These relate each radio port index with a channel in a stream.
  for (unsigned i_stream = 0, nof_streams = radio_config.tx_streams.size(); i_stream != nof_streams; ++i_stream) {
    const radio_configuration::stream& stream = radio_config.tx_streams[i_stream];
    for (unsigned i_channel = 0; i_channel != stream.channels.size(); ++i_channel) {
      // Save the stream and channel indexes for the port.
      tx_port_map.emplace_back(port_to_card_channel(i_stream, i_channel));
    }
  }

  for (unsigned i_stream = 0, nof_streams = radio_config.rx_streams.size(); i_stream != nof_streams; ++i_stream) {
    const radio_configuration::stream& stream = radio_config.rx_streams[i_stream];
    for (unsigned i_channel = 0; i_channel != stream.channels.size(); ++i_channel) {
      // Save the stream and channel indexes for the port.
      rx_port_map.emplace_back(port_to_card_channel(i_stream, i_channel));
    }
  }

  // Number of required cards.
  unsigned nof_cards = std::max(radio_config.tx_streams.size(), radio_config.rx_streams.size());

  fmt::print("Opening {} sidekiq cards...\n", nof_cards);

  srate_Hz = static_cast<uint64_t>(radio_config.sampling_rate_Hz);

  for (unsigned i_card = 0; i_card != nof_cards; ++i_card) {
    radio_sidekiq_card::card_description sidekiq_card_config;
    sidekiq_card_config.card_id     = i_card;
    sidekiq_card_config.stream_mode = skiq_rx_stream_mode_high_tput;
    sidekiq_card_config.srate_Hz    = radio_config.sampling_rate_Hz;
    sidekiq_card_config.packed_mode = radio_config.otw_format == radio_configuration::over_the_wire_format::SC12;

    // Determine the number of TX ports for the current card.
    unsigned nof_tx_ports =
        (radio_config.tx_streams.size() > i_card) ? radio_config.tx_streams[i_card].channels.size() : 0;

    for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
      radio_sidekiq_card::tx_port_params port_params;
      port_params.tx_gain_dB = radio_config.tx_streams[i_card].channels[i_port].gain_dB;
      port_params.tx_freq_Hz = radio_config.tx_streams[i_card].channels[i_port].freq.center_frequency_Hz;

      sidekiq_card_config.tx_port_config.emplace_back(port_params);
    }

    // Determine the number of RX ports for the current card.
    unsigned nof_rx_ports =
        (radio_config.rx_streams.size() > i_card) ? radio_config.rx_streams[i_card].channels.size() : 0;

    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      radio_sidekiq_card::rx_port_params port_params;
      port_params.rx_gain_dB = radio_config.rx_streams[i_card].channels[i_port].gain_dB;
      port_params.rx_freq_Hz = radio_config.rx_streams[i_card].channels[i_port].freq.center_frequency_Hz;

      sidekiq_card_config.rx_port_config.emplace_back(port_params);
    }

    cards.emplace_back(std::make_unique<radio_sidekiq_card>(async_executor, notifier, sidekiq_card_config));

    // Early return if the card was not successfully created.
    if (!cards.back()->is_successful()) {
      return;
    }
  }

  // Transition to successfully initialized.
  state.store(states::SUCCESSFUL_INIT);
}

bool radio_session_sidekiq_impl::set_tx_gain_unprotected(unsigned port_idx, double gain_dB)
{
  if (state.load() != states::SUCCESSFUL_INIT) {
    fmt::print("Error: radio session is not initialized.\n");
    return false;
  }

  if (port_idx >= tx_port_map.size()) {
    fmt::print(
        "Error: transmit port index ({}) exceeds the number of ports ({}).\n", port_idx, (int)tx_port_map.size());
    return false;
  }

  port_to_card_channel port_map = tx_port_map[port_idx];

  // Setup gain.
  if (!cards[port_map.first]->set_tx_gain(port_map.second, gain_dB)) {
    fmt::print("Error: setting gain for transmitter in card {} port {}.\n", port_map.first, port_map.second);
  }

  return true;
}

bool radio_session_sidekiq_impl::set_rx_gain_unprotected(unsigned port_idx, double gain_dB)
{
  if (state.load() != states::SUCCESSFUL_INIT) {
    fmt::print("Error: radio session is not initialized.\n");
    return false;
  }

  if (port_idx >= rx_port_map.size()) {
    fmt::print("Error: receive port index ({}) exceeds the number of ports ({}).\n", port_idx, (int)rx_port_map.size());
    return false;
  }

  port_to_card_channel port_map = rx_port_map[port_idx];

  // Setup gain.
  if (!cards[port_map.first]->set_rx_gain(port_map.second, gain_dB)) {
    fmt::print("Error: setting gain for receiver in card {} port {}.\n", port_map.first, port_map.second);
  }

  return true;
}

void radio_session_sidekiq_impl::start(ocudu::baseband_gateway_timestamp init_time)
{
  if (state.load() != states::SUCCESSFUL_INIT) {
    fmt::print("Error: radio session is not initialized.\n");
    return;
  }

  // Issue all receive streams to start if required.
  if (stream_start_required) {
    stream_start_required = false;

    for (auto& card : cards) {
      // Convert from RF to system timestamp.
      uint64_t sys_timestamp = (init_time * SKIQ_SYS_TIMESTAMP_FREQ) / srate_Hz;
      card->get_rx_stream().start(sys_timestamp);
    }
  }
}

void radio_session_sidekiq_impl::stop()
{
  // Attempt to transition into STOP state.
  states expected_state = states::SUCCESSFUL_INIT;
  bool   to_stop        = state.compare_exchange_weak(expected_state, states::STOP);

  if (!to_stop) {
    fmt::print("Error: attempting to stop non-initialized radio session\n");
    return;
  }

  // Stop each Sidekiq card.
  for (auto& card : cards) {
    card->stop();
  }

  // Close sidekiq SDK.
  skiq_exit();
}

baseband_gateway_timestamp radio_session_sidekiq_impl::read_current_time()
{
  uint64_t ts;

  // Read the RF timestamp of the first port of card 0.
  if (skiq_read_curr_rx_timestamp(0, skiq_rx_hdl_A1, &ts)) {
    fmt::print("Error reading radio time\n");
  }

  return ts;
}
