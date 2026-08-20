// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_tx_stream.h"
#include "ocudu/adt/span.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <array>

using namespace ocudu;

radio_difi_tx_stream::radio_difi_tx_stream(const stream_description& config_, radio_event_notifier& notifier_) :
  config(config_),
  notifier(notifier_),
  logger(ocudulog::fetch_basic_logger(config_.stream_id_str, false)),
  socket(logger)
{
  logger.set_level(config.log_level);

  // difi_data_packet_size() sizes anything that is not 8 as 16 bit, while the builder fills any
  // non-16 depth as 8 bit, so an unsupported depth would size and fill the payload inconsistently.
  // The depth comes from user configuration, so it is rejected here rather than asserted: an assert
  // is compiled out unless ASSERTS_ENABLED is set, which would leave a release build emitting
  // malformed packets.
  if (config.bit_depth != 8 && config.bit_depth != 16) {
    logger.error("Unsupported DIFI IQ bit depth {}, expected 8 or 16.", config.bit_depth);
    return;
  }

  // Size the packet buffer once for the largest fragment, so that transmit() never allocates on the radio thread.
  tx_buf.resize(difi_data_packet_size(config.bit_depth, MAX_PKT_SAMPLES).value());

  successful = true;
}

void radio_difi_tx_stream::transmit(const baseband_gateway_buffer_reader&        data,
                                    const baseband_gateway_transmitter_metadata& metadata)
{
  if (!socket.is_open()) {
    return;
  }

  const unsigned nof_samples = data.get_nof_samples();
  if (nof_samples == 0) {
    return;
  }

  // Underflow: gap between the expected next timestamp and the requested one.
  if (started && (metadata.ts > last_tx_end_ts)) {
    radio_event_notifier::event_description ev;
    ev.stream_id = static_cast<unsigned>(config.stream_id);
    ev.source    = radio_event_source::TRANSMIT;
    ev.type      = radio_event_type::UNDERFLOW;
    ev.timestamp = metadata.ts;
    notifier.on_radio_rt_event(ev);
  }
  last_tx_end_ts = metadata.ts + nof_samples;

  // Transmit the whole buffer, silence included: DIFI is a continuous timestamped stream, so omitting the silence a
  // hardware DAC would idle through leaves holes in it. The lower PHY zeroes the regions outside the transmit window
  // before handing the buffer over, so it is sent as it stands and no intermediate copy is needed.
  const span<const ci16_t> active = data[0];

  // Fragment across consecutive data packets of at most MAX_PKT_SAMPLES each: a whole slot exceeds both
  // the maximum UDP payload and the receive buffer of typical DIFI implementations.
  for (unsigned offset = 0; offset != nof_samples;) {
    const unsigned nof_chunk = std::min(MAX_PKT_SAMPLES, nof_samples - offset);
    // Each fragment carries the timestamp of its own first sample.
    const uint64_t chunk_ts = metadata.ts + offset;

    uint32_t full_secs = 0;
    uint64_t frac_ps   = 0;
    difi_ticks_to_time(static_cast<uint64_t>(static_cast<int64_t>(chunk_ts) + epoch_offset),
                       config.sample_rate_Hz,
                       full_secs,
                       frac_ps);

    // Periodic context re-send: send before the data packet at each interval boundary.
    if (pkts_since_ctx >= CTX_INTERVAL) {
      send_context_packet(full_secs, frac_ps);
      pkts_since_ctx = 0;
    }

    difi_data_packet_params p;
    p.stream_id = config.stream_id;
    p.full_secs = full_secs;
    p.frac_ps   = frac_ps;
    p.bit_depth = config.bit_depth;
    p.pkt_n     = data_pkt_n;

    const units::bytes pkt_bytes = difi_data_packet_size(config.bit_depth, nof_chunk);
    ocudu_assert(tx_buf.size() >= pkt_bytes.value(),
                 "Packet buffer of '{}' bytes is too small for a fragment of '{}' bytes",
                 tx_buf.size(),
                 pkt_bytes.value());

    build_difi_data_packet(tx_buf, p, active.subspan(offset, nof_chunk));

    if (!socket.send(span<const uint8_t>(tx_buf).first(pkt_bytes.value()))) {
      // Drop the rest of this buffer. The receiver detects the timestamp gap and zero-fills.
      return;
    }

    data_pkt_n = (data_pkt_n + 1U) & 0xfU;
    ++pkts_since_ctx;
    offset += nof_chunk;
  }
}

void radio_difi_tx_stream::send_context_packet(uint32_t full_secs, uint64_t frac_ps)
{
  last_ctx_full_secs = full_secs;
  last_ctx_frac_ps   = frac_ps;

  difi_context_packet_params p;
  p.stream_id      = config.stream_id;
  p.full_secs      = full_secs;
  p.frac_ps        = frac_ps;
  p.sample_rate_Hz = config.sample_rate_Hz;
  p.center_freq_Hz = config.center_freq_Hz;
  p.bit_depth      = config.bit_depth;
  p.rf_gain_dB     = config.rf_gain_dB;
  p.if_gain_dB     = config.if_gain_dB;
  p.ctx_pkt_n      = ctx_pkt_n;

  std::array<uint8_t, DIFI_CONTEXT_PACKET_SIZE.value()> buf;
  build_difi_context_packet(buf, p);

  if (!socket.send(buf)) {
    return;
  }

  ctx_pkt_n = (ctx_pkt_n + 1U) & 0xfU;
}

void radio_difi_tx_stream::start(baseband_gateway_timestamp init_time, int64_t epoch_offset_ticks)
{
  // A stream that failed construction must never put a packet on the wire.
  if (!successful) {
    return;
  }

  if (!socket.open_tx(config.ip, config.port)) {
    return;
  }

  epoch_offset   = epoch_offset_ticks;
  last_tx_end_ts = init_time;
  started        = true;

  uint32_t full_secs = 0;
  uint64_t frac_ps   = 0;
  difi_ticks_to_time(
      static_cast<uint64_t>(static_cast<int64_t>(init_time) + epoch_offset), config.sample_rate_Hz, full_secs, frac_ps);

  send_context_packet(full_secs, frac_ps);
}

void radio_difi_tx_stream::set_freq(double center_freq_Hz)
{
  config.center_freq_Hz = center_freq_Hz;
  if (socket.is_open()) {
    send_context_packet(last_ctx_full_secs, last_ctx_frac_ps);
  }
}

void radio_difi_tx_stream::set_gain(double rf_gain_dB, double if_gain_dB)
{
  config.rf_gain_dB = rf_gain_dB;
  config.if_gain_dB = if_gain_dB;
  if (socket.is_open()) {
    send_context_packet(last_ctx_full_secs, last_ctx_frac_ps);
  }
}

void radio_difi_tx_stream::stop()
{
  socket.close();
}
