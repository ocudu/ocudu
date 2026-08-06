// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_session_difi_impl.h"
#include "radio_difi_stream_config.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

/// Bit depth used when otw_format is DEFAULT or SC16.
static constexpr unsigned DEFAULT_BIT_DEPTH = 16;
/// Bit depth used when otw_format is SC8.
static constexpr unsigned SC8_BIT_DEPTH = 8;

radio_session_difi_impl::radio_session_difi_impl(const radio_configuration::radio& config,
                                                 task_executor&                    async_task_executor,
                                                 radio_event_notifier&             notifier) :
  logger(ocudulog::fetch_basic_logger("RF", false))
{
  ocudu_assert(config.tx_streams.size() == config.rx_streams.size(),
               "The number of transmit streams (i.e., {}) must equal the number of receive streams (i.e., {}).",
               config.tx_streams.size(),
               config.rx_streams.size());

  // Resolve bit depth from OTW format.
  unsigned bit_depth =
      (config.otw_format == radio_configuration::over_the_wire_format::SC8) ? SC8_BIT_DEPTH : DEFAULT_BIT_DEPTH;

  unsigned nof_streams = config.tx_streams.size();
  bb_gateways.reserve(nof_streams);

  for (unsigned stream_id = 0; stream_id != nof_streams; ++stream_id) {
    // Defaults: transmit to localhost:4991.
    auto tx_parsed = parse_difi_stream_args(config.tx_streams[stream_id].args, "127.0.0.1", 4991);
    if (!tx_parsed) {
      logger.error("Failed to parse DIFI Tx stream {} arguments.", stream_id);
      return;
    }

    // Defaults: bind all interfaces on port 4992.
    auto rx_parsed = parse_difi_stream_args(config.rx_streams[stream_id].args, "0.0.0.0", 4992);
    if (!rx_parsed) {
      logger.error("Failed to parse DIFI Rx stream {} arguments.", stream_id);
      return;
    }

    // Advertise the configured centre frequency and gain in the context packet. set_tx_freq() and
    // set_tx_gain() update them later, but nothing calls those during start-up.
    double tx_center_freq_Hz = 0.0;
    double tx_gain_dB        = 0.0;
    if (!config.tx_streams[stream_id].channels.empty()) {
      const auto& tx_channel = config.tx_streams[stream_id].channels.front();
      tx_center_freq_Hz      = tx_channel.freq.center_frequency_Hz;
      tx_gain_dB             = tx_channel.gain_dB;
    }

    radio_difi_tx_stream::stream_description tx_config = {.ip             = tx_parsed->ip,
                                                          .port           = tx_parsed->port,
                                                          .stream_id      = stream_id,
                                                          .bit_depth      = bit_depth,
                                                          .sample_rate_Hz = config.sampling_rate_Hz,
                                                          .center_freq_Hz = tx_center_freq_Hz,
                                                          .rf_gain_dB     = tx_gain_dB,
                                                          .if_gain_dB     = 0.0,
                                                          .stream_id_str  = "difi:tx:" + std::to_string(stream_id),
                                                          .log_level      = config.log_level};

    radio_difi_rx_stream::stream_description rx_config = {.ip             = rx_parsed->ip,
                                                          .port           = rx_parsed->port,
                                                          .stream_id      = stream_id,
                                                          .bit_depth      = bit_depth,
                                                          .sample_rate_Hz = config.sampling_rate_Hz,
                                                          .stream_id_str  = "difi:rx:" + std::to_string(stream_id),
                                                          .log_level      = config.log_level};

    logger.info("Creating DIFI stream {} — tx={}:{} rx={}:{} bit_depth={} srate={}Hz freq={}Hz gain={}dB",
                stream_id,
                tx_config.ip,
                tx_config.port,
                rx_config.ip,
                rx_config.port,
                bit_depth,
                config.sampling_rate_Hz,
                tx_center_freq_Hz,
                tx_gain_dB);

    auto& gateway =
        bb_gateways.emplace_back(std::make_unique<radio_difi_baseband_gateway>(notifier, tx_config, rx_config));

    if (!gateway->is_successful()) {
      logger.error("Failed to create DIFI baseband gateway for stream {}.", stream_id);
      return;
    }
  }

  successful = true;
}

baseband_gateway& radio_session_difi_impl::get_baseband_gateway(unsigned stream_id)
{
  ocudu_assert(stream_id < bb_gateways.size(),
               "Stream identifier (i.e., {}) exceeds the number of baseband gateways (i.e., {}).",
               stream_id,
               bb_gateways.size());
  return *bb_gateways[stream_id];
}

baseband_gateway_timestamp radio_session_difi_impl::read_current_time()
{
  if (bb_gateways.empty()) {
    return 0;
  }
  return bb_gateways[0]->get_rx_stream().get_sample_count();
}

void radio_session_difi_impl::start(baseband_gateway_timestamp init_time)
{
  for (auto& gateway : bb_gateways) {
    gateway->get_rx_stream().start(init_time);
  }
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().start(init_time);
  }
}

void radio_session_difi_impl::stop()
{
  for (auto& gateway : bb_gateways) {
    gateway->get_tx_stream().stop();
  }
  for (auto& gateway : bb_gateways) {
    gateway->get_rx_stream().stop();
  }
}

bool radio_session_difi_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  if (port_id >= bb_gateways.size()) {
    return false;
  }
  bb_gateways[port_id]->get_tx_stream().set_gain(gain_dB, 0.0);
  return true;
}

bool radio_session_difi_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  // Receive gain is not adjustable: context packets describing the stream are emitted by the
  // transmitter, so this side has no channel on which to advertise a change.
  return false;
}

bool radio_session_difi_impl::set_tx_freq(unsigned stream_id, double center_freq_Hz)
{
  if (stream_id >= bb_gateways.size()) {
    return false;
  }
  bb_gateways[stream_id]->get_tx_stream().set_freq(center_freq_Hz);
  return true;
}

bool radio_session_difi_impl::set_rx_freq(unsigned stream_id, double center_freq_Hz)
{
  // Not adjustable, for the same reason as set_rx_gain().
  return false;
}
