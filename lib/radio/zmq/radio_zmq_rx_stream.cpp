// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_zmq_rx_stream.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/sc_prod.h"

using namespace ocudu;

/// Scaling factor for converting from complex float to 16-bit complex integer.
static constexpr float scaling_factor_cf_to_ci16 = std::numeric_limits<int16_t>::max();
/// Alignment timeout. Waits this time before padding zeros.
static constexpr std::chrono::milliseconds RECEIVE_TS_ALIGN_TIMEOUT = std::chrono::milliseconds(100);

radio_zmq_rx_stream::radio_zmq_rx_stream(void*                         zmq_context,
                                         const stream_description&     config,
                                         task_executor&                async_executor_,
                                         radio_zmq_tx_align_interface& tx_align_,
                                         radio_event_notifier&         notification_handler) :
  tx_align(tx_align_), cf_buffer(config.buffer_size)
{
  channels.reserve(config.address.size());
  // For each channel...
  for (unsigned channel_id = 0, channel_id_end = config.address.size(); channel_id != channel_id_end; ++channel_id) {
    // Prepare configuration.
    radio_zmq_rx_channel::channel_description channel_description = {.socket_type    = config.socket_type,
                                                                     .address        = config.address[channel_id],
                                                                     .stream_id      = config.stream_id,
                                                                     .channel_id     = channel_id,
                                                                     .channel_id_str = config.stream_id_str + ":" +
                                                                                       std::to_string(channel_id),
                                                                     .log_level         = config.log_level,
                                                                     .trx_timeout_ms    = config.trx_timeout_ms,
                                                                     .linger_timeout_ms = config.linger_timeout_ms,
                                                                     .buffer_size       = config.buffer_size};

    // Create channel.
    auto& channel = channels.emplace_back(std::make_unique<radio_zmq_rx_channel>(
        zmq_context, channel_description, notification_handler, async_executor_));

    // Check if the channel construction was successful.
    if (!channel->is_successful()) {
      return;
    }
  }

  successful = true;
}

void radio_zmq_rx_stream::stop()
{
  for (auto& channel : channels) {
    channel->stop();
  }
}

void radio_zmq_rx_stream::start(baseband_gateway_timestamp init_time)
{
  sample_count = init_time;
  for (auto& channel : channels) {
    channel->start();
  }
}

baseband_gateway_receiver::metadata radio_zmq_rx_stream::receive(baseband_gateway_buffer_writer& data)
{
  // Make sure the number of data channels is coherent with the number of the stream channels.
  report_fatal_error_if_not(data.get_nof_channels() == channels.size(),
                            "Invalid number of channels ({}) expected {}.",
                            data.get_nof_channels(),
                            channels.size());

  // Prepare return metadata.
  baseband_gateway_receiver::metadata ret = {.ts = get_sample_count()};

  // Calculate transmit timestamp that has already expired.
  uint64_t passed_timestamp = ret.ts + data.get_nof_samples();

  // Align all transmit timestamps.
  tx_align.align(passed_timestamp, RECEIVE_TS_ALIGN_TIMEOUT);

  // Receive samples for each channel.
  for (unsigned channel_id = 0, channel_id_end = channels.size(); channel_id != channel_id_end; ++channel_id) {
    span<cf_t> view = span<cf_t>(cf_buffer).first(data.get_channel_buffer(channel_id).size());
    channels[channel_id]->receive(view);
    float gain = channel_gains[channel_id].load(std::memory_order_relaxed);
    if (gain != 1.0f) {
      ocuduvec::sc_prod(view, view, gain);
    }
    ocuduvec::convert(data.get_channel_buffer(channel_id), view, scaling_factor_cf_to_ci16);
  }

  // Increment the number of samples.
  sample_count += data.get_nof_samples();

  return ret;
}

void radio_zmq_rx_stream::set_channel_gain(unsigned channel_id, float gain_linear)
{
  ocudu_assert(channel_id < channels.size(),
               "Channel identifier (i.e., {}) exceeds the number of channels (i.e., {}).",
               channel_id,
               channels.size());
  channel_gains[channel_id].store(gain_linear, std::memory_order_relaxed);
}
