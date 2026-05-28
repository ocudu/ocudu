// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_session_zmq_impl.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

/// Default sockets send and receive timeout in milliseconds.
static constexpr unsigned DEFAULT_TRX_TIMEOUT_MS = 10;
/// Default linger timeout in milliseconds.
static constexpr unsigned DEFAULT_LINGER_TIMEOUT_MS = 0;
/// Default stream buffer size in samples.
static constexpr unsigned DEFAULT_STREAM_BUFFER_SIZE = 614400;

radio_session_zmq_impl::radio_session_zmq_impl(const radio_configuration::radio& config,
                                               task_executor&                    async_task_executor,
                                               radio_event_notifier&             notifier) :
  logger(ocudulog::fetch_basic_logger("RF", false))
{
  // Make sure the number of streams are equal.
  ocudu_assert(config.tx_streams.size() == config.rx_streams.size(),
               "The number of transmit streams (i.e., {}) must be equal to the number of receive streams (i.e., {}).",
               config.tx_streams.size(),
               config.rx_streams.size());

  // Make ZMQ context.
  zmq_context = ::zmq_ctx_new();
  if (zmq_context == nullptr) {
    logger.error("Failed to create ZMQ context. {}.", ::zmq_strerror(::zmq_errno()));
    return;
  }

  unsigned nof_streams = config.tx_streams.size();

  // Debug log level is only available if verbose keyword is in the device arguments.
  bool allow_log_level_debug = (config.args.find("verbose") != std::string::npos);

  // ZMQ logging in debug is extremely verbose. The following lines avoid debug level unless set to paranoid.
  ocudulog::basic_levels log_level = config.log_level;
  if (!allow_log_level_debug && (log_level >= ocudulog::basic_levels::debug)) {
    log_level = ocudulog::basic_levels::info;
  }

  bb_gateways.reserve(nof_streams);
  // Iterate for each transmission and reception stream.
  for (unsigned stream_id = 0; stream_id != nof_streams; ++stream_id) {
    const radio_configuration::stream& tx_radio_stream_config = config.tx_streams[stream_id];

    // Prepare transmit stream configuration.
    radio_zmq_tx_stream::stream_description tx_stream_config = {
        .socket_type = ZMQ_REP,
        .address =
            [&tx_radio_stream_config]() {
              std::vector<std::string> address;
              address.reserve(tx_radio_stream_config.channels.size());
              for (const auto& channel : tx_radio_stream_config.channels) {
                address.push_back(channel.args);
              }
              return address;
            }(),
        .stream_id         = stream_id,
        .stream_id_str     = "zmq:tx:" + std::to_string(stream_id),
        .log_level         = log_level,
        .trx_timeout_ms    = DEFAULT_TRX_TIMEOUT_MS,
        .linger_timeout_ms = DEFAULT_LINGER_TIMEOUT_MS,
        .buffer_size       = DEFAULT_STREAM_BUFFER_SIZE};

    const radio_configuration::stream& rx_radio_stream_config = config.rx_streams[stream_id];

    // Prepare receive stream configuration.
    radio_zmq_rx_stream::stream_description rx_stream_config = {
        .socket_type = ZMQ_REQ,
        .address =
            [&rx_radio_stream_config]() {
              std::vector<std::string> address;
              address.reserve(rx_radio_stream_config.channels.size());
              for (const auto& channel : rx_radio_stream_config.channels) {
                address.push_back(channel.args);
              }
              return address;
            }(),
        .stream_id         = stream_id,
        .stream_id_str     = "zmq:rx:" + std::to_string(stream_id),
        .log_level         = log_level,
        .trx_timeout_ms    = DEFAULT_TRX_TIMEOUT_MS,
        .linger_timeout_ms = DEFAULT_LINGER_TIMEOUT_MS,
        .buffer_size       = DEFAULT_STREAM_BUFFER_SIZE};

    // Create baseband gateway.
    auto& gateway = bb_gateways.emplace_back(std::make_unique<radio_zmq_baseband_gateway>(
        zmq_context, async_task_executor, notifier, tx_stream_config, rx_stream_config));

    // Make sure streams are created successfully.
    if (!gateway->get_tx_stream().is_successful() || !gateway->get_rx_stream().is_successful()) {
      return;
    }

    // Build port maps and apply initial gains from configuration.
    for (unsigned ch_id = 0, nof_ch = tx_radio_stream_config.channels.size(); ch_id != nof_ch; ++ch_id) {
      tx_port_map.emplace_back(port_to_stream_channel(stream_id, ch_id));
      gateway->get_tx_stream().set_channel_gain(
          ch_id, convert_dB_to_amplitude(static_cast<float>(tx_radio_stream_config.channels[ch_id].gain_dB)));
    }
    for (unsigned ch_id = 0, nof_ch = rx_radio_stream_config.channels.size(); ch_id != nof_ch; ++ch_id) {
      rx_port_map.emplace_back(port_to_stream_channel(stream_id, ch_id));
      gateway->get_rx_stream().set_channel_gain(
          ch_id, convert_dB_to_amplitude(static_cast<float>(rx_radio_stream_config.channels[ch_id].gain_dB)));
    }
  }

  successful = true;
}

radio_session_zmq_impl::~radio_session_zmq_impl()
{
  // Destroy transmit and receive streams prior to ZMQ context destruction.
  bb_gateways.clear();

  // Destroy ZMQ context.
  if (zmq_context != nullptr) {
    ::zmq_ctx_shutdown(zmq_context);
    ::zmq_ctx_destroy(zmq_context);
    zmq_context = nullptr;
  }
}

void radio_session_zmq_impl::stop()
{
  // Signal stop for each transmit stream.
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().stop();
  }

  // Signal stop for each receive stream.
  for (auto& gateway : bb_gateways) {
    gateway->get_rx_stream().stop();
  }
}

bool radio_session_zmq_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  if (port_id >= tx_port_map.size()) {
    logger.error("Invalid TX port index ({}).", port_id);
    return false;
  }
  auto [stream_id, channel_id] = tx_port_map[port_id];
  bb_gateways[stream_id]->get_tx_stream().set_channel_gain(channel_id,
                                                           convert_dB_to_amplitude(static_cast<float>(gain_dB)));
  return true;
}

bool radio_session_zmq_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  if (port_id >= rx_port_map.size()) {
    logger.error("Invalid RX port index ({}).", port_id);
    return false;
  }
  auto [stream_id, channel_id] = rx_port_map[port_id];
  bb_gateways[stream_id]->get_rx_stream().set_channel_gain(channel_id,
                                                           convert_dB_to_amplitude(static_cast<float>(gain_dB)));
  return true;
}

void radio_session_zmq_impl::start(baseband_gateway_timestamp start_time)
{
  for (auto& gateway : bb_gateways) {
    gateway->get_rx_stream().start(start_time);
  }
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().start(start_time);
  }
}

baseband_gateway_timestamp radio_session_zmq_impl::read_current_time()
{
  return 0;
}

bool radio_session_zmq_impl::set_tx_freq(unsigned stream_id, double center_freq_Hz)
{
  return false;
}

bool radio_session_zmq_impl::set_rx_freq(unsigned stream_id, double center_freq_Hz)
{
  return false;
}
