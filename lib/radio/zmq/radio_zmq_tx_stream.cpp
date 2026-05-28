// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_zmq_tx_stream.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/sc_prod.h"

using namespace ocudu;

/// Scaling factor for converting from 16-bit complex integer to complex float.
static constexpr float scaling_factor_ci16_to_cf = std::numeric_limits<int16_t>::max();
/// Alignment timeout. Waits this time before padding zeros.
static constexpr std::chrono::milliseconds TRANSMIT_TS_ALIGN_TIMEOUT = std::chrono::milliseconds(0);

radio_zmq_tx_stream::radio_zmq_tx_stream(void*                     zmq_context,
                                         const stream_description& config,
                                         task_executor&            async_executor_,
                                         radio_event_notifier&     notification_handler_) :
  notification_handler(notification_handler_), cf_buffer(config.buffer_size)
{
  channels.reserve(config.address.size());
  // For each channel...
  for (unsigned channel_id = 0, channel_id_end = config.address.size(); channel_id != channel_id_end; ++channel_id) {
    // Prepare configuration.
    radio_zmq_tx_channel::channel_description channel_description = {.socket_type    = config.socket_type,
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
    auto& channel = channels.emplace_back(std::make_unique<radio_zmq_tx_channel>(
        zmq_context, channel_description, notification_handler_, async_executor_));

    // Check if the channel construction was successful.
    if (!channel->is_successful()) {
      return;
    }
  }

  successful = true;
}

void radio_zmq_tx_stream::start(ocudu::baseband_gateway_timestamp init_time)
{
  for (auto& channel : channels) {
    channel->start(init_time);
  }
}

void radio_zmq_tx_stream::stop()
{
  for (auto& channel : channels) {
    channel->stop();
  }
}

bool radio_zmq_tx_stream::align(baseband_gateway_timestamp timestamp, std::chrono::milliseconds timeout)
{
  // Returns true if at least one channel is in the past.
  bool timestamp_passed = false;
  for (auto& channel : channels) {
    timestamp_passed = timestamp_passed || channel->align(timestamp, timeout);
  }
  return timestamp_passed;
}

void radio_zmq_tx_stream::transmit(const baseband_gateway_buffer_reader&        data,
                                   const baseband_gateway_transmitter_metadata& md)
{
  report_fatal_error_if_not(data.get_nof_channels() == channels.size(),
                            "Invalid number of channels ({}) expected {}.",
                            data.get_nof_channels(),
                            channels.size());

  // Align stream to the new timestamp.
  bool timestamp_passed = align(md.ts, TRANSMIT_TS_ALIGN_TIMEOUT);

  // Notify that a timestamp is late.
  if (timestamp_passed) {
    radio_event_notifier::event_description event_description = {.stream_id  = std::nullopt,
                                                                 .channel_id = std::nullopt,
                                                                 .source     = radio_event_source::TRANSMIT,
                                                                 .type       = radio_event_type::LATE,
                                                                 .timestamp  = std::nullopt};
    notification_handler.on_radio_rt_event(event_description);
    return;
  }

  for (unsigned channel_id = 0, channel_id_end = channels.size(); channel_id != channel_id_end; ++channel_id) {
    span<cf_t> view = span<cf_t>(cf_buffer).first(data.get_channel_buffer(channel_id).size());
    ocuduvec::convert(view, data.get_channel_buffer(channel_id), scaling_factor_ci16_to_cf);
    float gain = channel_gains[channel_id].load(std::memory_order_relaxed);
    if (gain != 1.0f) {
      ocuduvec::sc_prod(view, view, gain);
    }
    channels[channel_id]->transmit(view);
  }
}

void radio_zmq_tx_stream::set_channel_gain(unsigned channel_id, float gain_linear)
{
  ocudu_assert(channel_id < channels.size(),
               "Channel identifier (i.e., {}) exceeds the number of channels (i.e., {}).",
               channel_id,
               channels.size());
  channel_gains[channel_id].store(gain_linear, std::memory_order_relaxed);
}
