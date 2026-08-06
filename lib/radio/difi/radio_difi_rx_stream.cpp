// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_rx_stream.h"
#include "radio_difi_data_packet.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/zero.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>

using namespace ocudu;

/// Maximum UDP payload size in bytes.
static constexpr size_t MAX_UDP_PAYLOAD = 65507;

/// DIFI packet type for IF data with stream identifier.
static constexpr uint8_t DIFI_PKT_TYPE_DATA = 0x1U;

/// Largest number of IQ samples one datagram can carry, at the 8 bit depth where a sample takes two bytes.
static constexpr size_t MAX_SAMPLES_PER_DATAGRAM = (MAX_UDP_PAYLOAD - DIFI_DATA_HEADER_SIZE.value()) / 2;

/// \brief Largest offset between the transmitter epoch and ours that is treated as a shared clock.
///
/// A peer locked onto our timestamps lands within milliseconds; its own epoch is seconds or years away.
static constexpr double SHARED_EPOCH_TOLERANCE_S = 0.1;

static inline uint32_t read_u32_be(const uint8_t* p)
{
  return (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) |
         (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

static inline uint64_t read_u64_be(const uint8_t* p)
{
  return (static_cast<uint64_t>(read_u32_be(p)) << 32) | read_u32_be(p + 4);
}

/// Unpacks \c nof_samples interleaved IQ samples from \c payload into \c dst.
static void unpack_samples(const uint8_t* payload, unsigned nof_samples, unsigned bit_depth, span<ci16_t> dst)
{
  // The IQ payload is in host byte order; only the metadata fields are big endian.
  if (bit_depth == 16) {
    for (unsigned i = 0; i != nof_samples; ++i) {
      int16_t re = 0;
      int16_t im = 0;
      std::memcpy(&re, payload + i * 4, 2);
      std::memcpy(&im, payload + i * 4 + 2, 2);
      dst[i] = ci16_t(re, im);
    }
    return;
  }

  // 8-bit samples are signed bytes; scale back to int16 by shifting left 8.
  for (unsigned i = 0; i != nof_samples; ++i) {
    const int16_t re = static_cast<int16_t>(static_cast<int8_t>(payload[i * 2 + 0])) << 8;
    const int16_t im = static_cast<int16_t>(static_cast<int8_t>(payload[i * 2 + 1])) << 8;
    dst[i]           = ci16_t(re, im);
  }
}

radio_difi_rx_stream::radio_difi_rx_stream(const stream_description& config_, radio_event_notifier& notifier_) :
  config(config_),
  notifier(notifier_),
  logger(ocudulog::fetch_basic_logger(config_.stream_id_str, false)),
  socket(logger),
  rx_buf(MAX_UDP_PAYLOAD)
{
  logger.set_level(config.log_level);

  // Reserve the largest tail a datagram can leave behind, so receive() never allocates.
  carry_samples.reserve(MAX_SAMPLES_PER_DATAGRAM);
  successful = true;
}

baseband_gateway_receiver::metadata radio_difi_rx_stream::receive(baseband_gateway_buffer_writer& data)
{
  const unsigned nof_requested = data.get_nof_samples();
  const uint64_t base_ts       = sample_count.load(std::memory_order_relaxed);
  span<ci16_t>   channel       = data[0];

  const unsigned bytes_per_sample = (config.bit_depth == 8) ? 2U : 4U;

  // Wall-clock span the requested samples represent. This call must occupy that span: receive() is the
  // only thing pacing the lower physical layer, whose uplink chain reschedules itself with no throttle
  // and whose downlink waits on the timestamp published here. Returning late throttles the downlink too.
  const auto budget = std::chrono::nanoseconds(
      (config.sample_rate_Hz > 0.0)
          ? static_cast<int64_t>(static_cast<double>(nof_requested) * 1e9 / config.sample_rate_Hz)
          : 0);
  const auto deadline = std::chrono::steady_clock::now() + budget;

  unsigned filled       = 0;
  bool     gap_reported = false;

  // Resolves where a block of samples starting at local_ts belongs in this buffer. Zero-fills any gap
  // ahead of it, and returns how many leading samples are stale, equal to the block size if all are.
  auto position_block = [&](int64_t local_ts, unsigned nof_block) -> unsigned {
    const int64_t delta = local_ts - static_cast<int64_t>(base_ts + filled);

    if (delta < 0) {
      const auto stale = static_cast<uint64_t>(-delta);
      return (stale >= nof_block) ? nof_block : static_cast<unsigned>(stale);
    }

    if (delta > 0) {
      // Datagrams were lost. Zero-fill the missing span so what did arrive stays at its correct position.
      const auto gap = static_cast<unsigned>(std::min<uint64_t>(delta, nof_requested - filled));
      ocuduvec::zero(channel.subspan(filled, gap));
      if (!gap_reported) {
        radio_event_notifier::event_description ev;
        ev.stream_id = static_cast<unsigned>(config.stream_id);
        ev.source    = radio_event_source::RECEIVE;
        ev.type      = radio_event_type::OVERFLOW;
        ev.timestamp = base_ts + filled;
        notifier.on_radio_rt_event(ev);
        gap_reported = true;
      }
      filled += gap;
    }

    return 0;
  };

  // Place the tail of a packet that straddled the end of the previous buffer before reading anything new.
  if (!carry_samples.empty()) {
    const auto     nof_carry = static_cast<unsigned>(carry_samples.size());
    const unsigned skip      = position_block(static_cast<int64_t>(carry_ts), nof_carry);
    if (skip < nof_carry && filled < nof_requested) {
      const unsigned nof_write = std::min(nof_carry - skip, nof_requested - filled);
      ocuduvec::copy(channel.subspan(filled, nof_write), span<const ci16_t>(carry_samples).subspan(skip, nof_write));
      filled += nof_write;
      if (skip + nof_write < nof_carry) {
        carry_samples.erase(carry_samples.begin(), carry_samples.begin() + skip + nof_write);
        carry_ts += skip + nof_write;
      } else {
        carry_samples.clear();
      }
    } else {
      carry_samples.clear();
    }
  }

  while (filled != nof_requested) {
    // The deadline bounds the whole call. Past it the wait drops to zero, which still drains anything
    // already queued, so a receiver that has fallen behind catches up instead of discarding a backlog.
    const auto now  = std::chrono::steady_clock::now();
    auto       wait = std::chrono::microseconds(0);
    if (now < deadline) {
      wait = std::chrono::duration_cast<std::chrono::microseconds>(deadline - now);
    }

    if (!socket.wait_readable(wait)) {
      break;
    }

    const auto received = socket.recv(rx_buf);
    if (!received.has_value()) {
      break;
    }

    const size_t   len = received.value().size();
    const uint8_t* buf = received.value().data();
    if (len < DIFI_DATA_HEADER_SIZE.value()) {
      continue;
    }

    // Packet type lives in bits[31:28]. Context and unknown types carry no samples.
    const uint32_t header = read_u32_be(buf + 0);
    if (static_cast<uint8_t>((header >> 28) & 0xfU) != DIFI_PKT_TYPE_DATA) {
      continue;
    }

    if (read_u32_be(buf + 4) != config.stream_id) {
      continue;
    }

    const uint64_t pkt_ts = difi_time_to_ticks(read_u32_be(buf + 16), read_u64_be(buf + 20), config.sample_rate_Hz);

    if (!ts_offset.has_value()) {
      // Trust absolute time when the transmitter is plausibly on our clock. Translating a peer that is
      // already on it would place its uplink an arbitrary distance away, since a receiver derives the
      // subframe of a buffer purely from its timestamp.
      const int64_t delta     = static_cast<int64_t>(base_ts + filled) - static_cast<int64_t>(pkt_ts);
      const int64_t plausible = static_cast<int64_t>(config.sample_rate_Hz * SHARED_EPOCH_TOLERANCE_S);
      ts_offset               = (config.latch_peer_epoch || (std::abs(delta) > plausible)) ? delta : 0;
    }

    const uint8_t* payload  = buf + DIFI_DATA_HEADER_SIZE.value();
    const auto     nof_pkt  = static_cast<unsigned>((len - DIFI_DATA_HEADER_SIZE.value()) / bytes_per_sample);
    const int64_t  local_ts = static_cast<int64_t>(pkt_ts) + *ts_offset;

    const unsigned skip = position_block(local_ts, nof_pkt);
    if (skip >= nof_pkt) {
      continue;
    }
    if (filled == nof_requested) {
      // The gap alone filled the buffer; this packet belongs to the next one.
      const unsigned nof_carry = nof_pkt - skip;
      carry_samples.resize(nof_carry);
      unpack_samples(payload + skip * bytes_per_sample, nof_carry, config.bit_depth, carry_samples);
      carry_ts = static_cast<uint64_t>(local_ts) + skip;
      break;
    }

    const unsigned nof_write = std::min(nof_pkt - skip, nof_requested - filled);
    unpack_samples(payload + skip * bytes_per_sample, nof_write, config.bit_depth, channel.subspan(filled, nof_write));
    filled += nof_write;

    if (skip + nof_write < nof_pkt) {
      // The packet runs past the end of this buffer. Keep the tail for the next call.
      const unsigned nof_carry = nof_pkt - skip - nof_write;
      carry_samples.resize(nof_carry);
      unpack_samples(payload + (skip + nof_write) * bytes_per_sample, nof_carry, config.bit_depth, carry_samples);
      carry_ts = static_cast<uint64_t>(local_ts) + skip + nof_write;
    }
  }

  // Zero-fill whatever the transmitter did not supply, so the caller always sees a full buffer.
  if (filled != nof_requested) {
    ocuduvec::zero(channel.subspan(filled, nof_requested - filled));
  }

  // The timeline advances by the whole buffer regardless of how much arrived, matching how the caller
  // computes the next expected timestamp.
  sample_count.store(base_ts + nof_requested, std::memory_order_relaxed);

  return metadata{.ts = base_ts};
}

void radio_difi_rx_stream::start(baseband_gateway_timestamp init_time)
{
  sample_count.store(init_time, std::memory_order_relaxed);

  // Drop any latched epoch and leftover samples: the next packet re-locks onto the transmitter.
  ts_offset.reset();
  carry_samples.clear();
  carry_ts = 0;

  socket.open_rx(config.ip, config.port);
}

void radio_difi_rx_stream::stop()
{
  socket.close();
}
