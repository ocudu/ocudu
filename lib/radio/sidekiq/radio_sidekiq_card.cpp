// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_sidekiq_card.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;

static double to_MHz(double value_Hz)
{
  return value_Hz * 1e-6;
}

bool radio_sidekiq_card::set_tx_lo_freq(unsigned i_channel, double freq_Hz)
{
  if (state.load() != card_states::UNINITIALIZED) {
    return false;
  }

  skiq_tx_hdl_t port_handle = card_params.tx_param[i_channel].handle;

  if (skiq_write_tx_LO_freq(card_id, port_handle, static_cast<uint64_t>(freq_Hz))) {
    fmt::print("Error setting card {} port {} Tx Lo frequency\n", card_id, i_channel);
    return false;
  }

  uint64_t actual_tx_freq;
  double   actual_tuned_freq;
  if (skiq_read_tx_LO_freq(card_id, port_handle, &actual_tx_freq, &actual_tuned_freq)) {
    fmt::print("Error reading card {} port {} Tx Lo frequency\n", card_id, i_channel);
    return false;
  }

  logger.debug("Setting sidekiq card {} channel {} Tx frequency to {} MHz, actually got {} MHz.",
               card_id,
               i_channel,
               to_MHz(freq_Hz),
               to_MHz(static_cast<double>(actual_tx_freq)));

  fmt::print("Setting sidekiq card {} channel {} Tx frequency to {} MHz, actually got {} MHz.\n",
             card_id,
             i_channel,
             to_MHz(freq_Hz),
             to_MHz(static_cast<double>(actual_tx_freq)));

  return true;
}

bool radio_sidekiq_card::set_rx_lo_freq(unsigned i_channel, double freq_Hz)
{
  if (state.load() != card_states::UNINITIALIZED) {
    return false;
  }

  skiq_rx_hdl_t port_handle = card_params.rx_param[i_channel].handle;

  if (skiq_write_rx_LO_freq(card_id, port_handle, static_cast<uint64_t>(freq_Hz))) {
    fmt::print(stderr, "Error setting card {} port {} Rx Lo frequency\n", card_id, i_channel);
    return false;
  }

  uint64_t actual_rx_freq;
  double   actual_tuned_freq;
  if (skiq_read_rx_LO_freq(card_id, port_handle, &actual_rx_freq, &actual_tuned_freq)) {
    fmt::print("Error reading card {} port {} Rx Lo frequency\n", card_id, i_channel);
    return false;
  }

  logger.debug("Setting sidekiq card {} channel {} Rx frequency to {} MHz, actually got {} MHz.",
               card_id,
               i_channel,
               to_MHz(freq_Hz),
               to_MHz(static_cast<double>(actual_rx_freq)));

  fmt::print("Setting sidekiq card {} channel {} Rx frequency to {} MHz, actually got {} MHz.\n",
             card_id,
             i_channel,
             to_MHz(freq_Hz),
             to_MHz(static_cast<double>(actual_rx_freq)));

  return true;
}

radio_sidekiq_card::radio_sidekiq_card(task_executor&          async_executor,
                                       radio_event_notifier&   notifier,
                                       const card_description& card_config) :
  card_id(card_config.card_id),
  nof_tx_ports(card_config.tx_port_config.size()),
  nof_rx_ports(card_config.rx_port_config.size()),
  logger(ocudulog::fetch_basic_logger("RF"))
{
  ocudu_assert((card_config.stream_mode != skiq_rx_stream_mode_t::skiq_rx_stream_mode_low_latency) ||
                   !card_config.packed_mode,
               "Packed 12-bit mode cannot be used with low-latency stream mode");

  // Initialize Sidekiq driver.
  auto card_id_ = static_cast<uint8_t>(card_id);
  if (skiq_init(skiq_xport_type_pcie, skiq_xport_init_level_full, &card_id_, 1)) {
    fmt::print("Unable to initialise card {}\n", card_id);
    return;
  }

  // Read card parameters.
  if (skiq_read_parameters(card_id_, &card_params)) {
    fmt::print("Error Reading card {} param\n", card_id);
    return;
  }

  // Check number of RX channels.
  if (card_params.rf_param.num_rx_channels < nof_rx_ports) {
    fmt::print("Card {} does not support {} Rx channels\n", card_id, nof_rx_ports);
    return;
  }

  // Check number of TX channels
  if (card_params.rf_param.num_tx_channels < nof_tx_ports) {
    fmt::print("Card {} does not support {} Tx channels\n", card_id, nof_tx_ports);
    return;
  }

  // Set the RX timeout.
  if (skiq_set_rx_transfer_timeout(card_id_, rx_timeout_us)) {
    fmt::print("Error Setting Rx transfer timeout\n");
    return;
  }

  // Set packing sample mode.
  if (skiq_write_iq_pack_mode(card_id_, card_config.packed_mode)) {
    fmt::print("Error Setting IQ pack mode\n");
    return;
  }

  // IQ order of the samples in the RF block. In unpacked mode, conversion from ci16_t into 16-bit integer values
  // respects the original ordering (IQ), and the samples are written as such into the RF block. In packed mode, the
  // default QI ordering is used, since the packing format is defined for this IQ order.
  skiq_iq_order_t iq_order =
      card_config.packed_mode ? skiq_iq_order_t::skiq_iq_order_qi : skiq_iq_order_t::skiq_iq_order_iq;

  // Set IQ order.
  if (skiq_write_iq_order_mode(card_id_, iq_order)) {
    fmt::print("Error Setting IQ order mode\n");
    return;
  }

  // Set the control output bits to include the gain.
  if (skiq_write_rfic_control_output_config(
          card_id, RFIC_CONTROL_OUTPUT_MODE_GAIN_CONTROL_RXA1, RFIC_CONTROL_OUTPUT_MODE_GAIN_BITS) != 0) {
    fmt::print("Unable to configure card {} the RF IC control output (A1)\n", card_id);
    return;
  }

  // Set channel mode.
  channel_mode = (nof_tx_ports > 1) ? skiq_chan_mode_t::skiq_chan_mode_dual : skiq_chan_mode_t::skiq_chan_mode_single;
  if (skiq_write_chan_mode(card_id_, channel_mode)) {
    fmt::print("Error Setting card {} channel mode\n", card_id);
    return;
  }

  // Set Rx streaming mode.
  if (skiq_write_rx_stream_mode(card_id_, card_config.stream_mode)) {
    fmt::print("Error setting Rx stream mode\n");
    return;
  }

  radio_sidekiq_tx_stream::stream_description tx_stream_config;
  tx_stream_config.card_id      = card_id;
  tx_stream_config.stream_mode  = card_config.stream_mode;
  tx_stream_config.channel_mode = channel_mode;
  tx_stream_config.packed_mode  = card_config.packed_mode;
  for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
    // Set the stream Tx port handles.
    tx_stream_config.tx_port_handles.push_back(card_params.tx_param[i_port].handle);

    // Set the sample rate of each Tx port.
    logger.debug("Setting Tx Rate to {} MHz.", to_MHz(card_config.srate_Hz));
    if (skiq_write_tx_sample_rate_and_bandwidth(card_id_,
                                                card_params.tx_param[i_port].handle,
                                                static_cast<uint32_t>(card_config.srate_Hz),
                                                static_cast<uint32_t>(card_config.srate_Hz))) {
      fmt::print("Error card {} port {}: setting the Tx sample rate\n", card_id, i_port);
      return;
    }

    // Set the center frequency.
    if (!radio_sidekiq_card::set_tx_lo_freq(i_port, card_config.tx_port_config[i_port].tx_freq_Hz)) {
      return;
    }

    // Set the Tx gain.
    if (!set_tx_gain(i_port, card_config.tx_port_config[i_port].tx_gain_dB)) {
      return;
    }
  }

  radio_sidekiq_rx_stream::stream_description rx_stream_config;
  rx_stream_config.card_id     = card_id;
  rx_stream_config.stream_mode = card_config.stream_mode;
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    // Set the stream Rx port handles.
    rx_stream_config.rx_port_handles.push_back(card_params.rx_param[i_port].handle);
    rx_stream_config.packed_mode = card_config.packed_mode;

    // Set the sample rate of each Rx port.
    logger.debug("Setting Rx Rate to {} MHz.", to_MHz(card_config.srate_Hz));
    if (skiq_write_rx_sample_rate_and_bandwidth(card_id_,
                                                card_params.rx_param[i_port].handle,
                                                static_cast<uint32_t>(card_config.srate_Hz),
                                                static_cast<uint32_t>(card_config.srate_Hz))) {
      fmt::print("Error card {} port {}: setting the Rx sample rate\n", card_id, i_port);
      return;
    }

    // Set the center frequency.
    if (!set_rx_lo_freq(i_port, card_config.rx_port_config[i_port].rx_freq_Hz)) {
      return;
    }

    // Set the Rx gain.
    if (!set_rx_gain(i_port, card_config.rx_port_config[i_port].rx_gain_dB)) {
      return;
    }
  }

  // Reset the card timestamp.
  if (skiq_update_timestamps(card_id_, 0U)) {
    fmt::print("Error updating timestamp\n");
  }

  // Create and initialize streams.
  tx_stream = std::make_unique<radio_sidekiq_tx_stream>(tx_stream_config, notifier);
  if (!tx_stream || !tx_stream->init_successful()) {
    fmt::print("Error initializing TX stream for card {}\n", card_id);
  }

  rx_stream = std::make_unique<radio_sidekiq_rx_stream>(rx_stream_config, notifier);
  if (!rx_stream || !rx_stream->init_successful()) {
    fmt::print("Error initializing RX stream for card {}\n", card_id);
  }

  state.store(card_states::SUCCESSFUL_INIT);
}

bool radio_sidekiq_card::set_tx_gain(unsigned i_channel, double gain_dB)
{
  if (state.load() == card_states::STOP) {
    return false;
  }

  logger.debug("Setting channel {} Tx gain to {:.2f} dB.", i_channel, gain_dB);

  ocudu_assert(gain_dB >= 0.0, "TX gain cannot be negative.");

  interval<double> atten_dB_range = {0.25 * card_params.tx_param->atten_quarter_db_min,
                                     0.25 * card_params.tx_param->atten_quarter_db_max};

  // Calculate attenuation:
  // - 0dB attenuation -> Maximum gain;
  // - 0dB gain -> Maximum attenuation;
  interval<double> gain_range_dB = {0.0, atten_dB_range.length()};
  double           att_dB        = atten_dB_range.stop() - gain_dB;

  // Check gain range
  if (!atten_dB_range.contains(att_dB)) {
    fmt::print("Error card {} channel {}: the selected gain (i.e. {:.1f} dB) is out of the range {}.\n",
               card_id,
               i_channel,
               gain_dB,
               gain_range_dB);
    return false;
  }

  // Calculate attenuation index.
  auto att_index = static_cast<uint16_t>(floor(att_dB * 4));

  // Set gain per port.
  if (skiq_write_tx_attenuation(card_id, card_params.tx_param[i_channel].handle, att_index)) {
    fmt::print("Error: setting card {} port {} Tx attenuation.\n", card_id, i_channel);
    return false;
  }

  return true;
}

bool radio_sidekiq_card::set_rx_gain(unsigned i_channel, double gain_dB)
{
  if (state.load() == card_states::STOP) {
    return false;
  }

  logger.debug("Setting channel {} Rx gain to {:.2f} dB.", i_channel, gain_dB);

  ocudu_assert(gain_dB >= 0.0, "RX gain cannot be negative.");

  // From Sidekiq API doc:
  //
  // For Sidekiq mPCIe (sidekiq_mpcie), Sidekiq M.2 (sidekiq_m2), Sidekiq Stretch (sidekiq_m2_2280), Sidekiq Z2
  //(sidekiq_z2), and Matchstiq Z3u (sidekiq_z3u) each increment of the gain index value results in approxi-
  // mately 1 dB of gain, with approximately 76 dB of total gain available. For details on the gain table,
  // refer to p. 37 of AD9361 Reference Manual UG-570.
  double gain_dB_step = 1.0;

  // For Sidekiq X4 (skiq_x4) and Matchstiq X40 (skiq_x40), each receiver has 30 dB of total gain available,
  // where an increment of 1 in the gain index results in approximately 0.5 dB increase. For details on the
  // receiver datapath and gain control blocks, refer to the "Receiver Datapath" on p. 125 of the ADRV9008-
  // 1/ADRV9008-2/ADRV9009 Hardware Reference Manual (UG-1295)
  if (card_params.rx_param->gain_index_min == 195) {
    // Sidekiq X4 minimum gain index is 195.
    gain_dB_step = 0.5;
  }

  // Expected gain range in decibels.
  interval<uint16_t> gain_dB_range(
      0.0, gain_dB_step * (card_params.rx_param->gain_index_max - gain_dB_step * card_params.rx_param->gain_index_min));

  // Check gain range.
  if (!gain_dB_range.contains(gain_dB)) {
    fmt::print("Error card {} channel {}: the selected gain (i.e. {:.1f} dB) is out of the range {}.\n",
               card_id,
               i_channel,
               gain_dB,
               gain_dB_range);
    return false;
  }

  // Calculate the gain index. The minimum index is mapped to 0dB.
  auto gain_idx = static_cast<uint16_t>(floor(gain_dB / gain_dB_step)) + card_params.rx_param->gain_index_min;

  // Set the RX gain.
  if (skiq_write_rx_gain(card_id, card_params.rx_param[i_channel].handle, gain_idx)) {
    fmt::print("Error: setting card {} port {} Rx gain.\n", card_id, i_channel, gain_idx);
    return false;
  }

  return true;
}

void radio_sidekiq_card::stop()
{
  // Attempt to transition into STOP state.
  card_states expected_state = card_states::SUCCESSFUL_INIT;
  bool        to_stop        = state.compare_exchange_weak(expected_state, card_states::STOP);

  if (!to_stop) {
    fmt::print("Error: attempting to stop non-initialized radio card\n");
    return;
  }

  // Resquest the streams to stop.
  tx_stream->stop();
  rx_stream->stop();

  // Wait for the streams to stop.
  tx_stream->wait_stop();
  rx_stream->wait_stop();

  // Unlock the card.
  auto card_id_ = static_cast<uint8_t>(card_id);
  if (skiq_disable_cards(&card_id_, 1)) {
    fmt::print("Unable to disable card {}\n", card_id);
  }
}
