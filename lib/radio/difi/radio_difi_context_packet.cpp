// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_context_packet.h"
#include "radio_difi_packing.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

///
/// DIFI constants.
///

/// Header template: type=0x4 (context), class-id present, integer-seconds timestamp mode.
static constexpr uint32_t CONTEXT_STATIC_BITS = 0x49e00000U;
/// Fixed packet size in 32-bit words (108 / 4 = 27).
static constexpr uint32_t CONTEXT_SIZE_WORDS = 27U;
/// DIFI Organizational Unique Identifier (OUI) for the standard context packet.
static constexpr uint32_t OUI = 0x6a621eU;
/// Device class field within the class ID (0 = unspecified).
static constexpr uint32_t DEVICE_CLASS = 0U;
/// Reference point identifier (fixed DIFI value).
static constexpr uint32_t REF_POINT = 0xfbb98000U;
/// Default state-and-event word.
static constexpr uint32_t DEFAULT_STATE_AND_EVENTS = 0x9ff00000U;
/// Timestamp adjustment: 10 µs expressed in femtoseconds.
static constexpr uint64_t TIMESTAMP_ADJ_FS = 10000000000ULL;
/// payload_format word for 8-bit IQ samples.
static constexpr uint64_t PAYLOAD_FMT_8BIT = 0xa00003c700000000ULL;
/// payload_format word for 16-bit IQ samples.
static constexpr uint64_t PAYLOAD_FMT_16BIT = 0xa00007cf00000000ULL;

///
/// Packet builder.
///

void ocudu::build_difi_context_packet(span<uint8_t> buf, const difi_context_packet_params& p)
{
  ocudu_assert(buf.size() >= DIFI_CONTEXT_PACKET_SIZE.value(),
               "Buffer of '{}' bytes is too small for a DIFI context packet of '{}' bytes",
               buf.size(),
               DIFI_CONTEXT_PACKET_SIZE.value());

  uint8_t* out = buf.data();
  ocuduvec::zero(buf.first(DIFI_CONTEXT_PACKET_SIZE.value()));

  // Offset 0: header word — static bits | pkt_n in bits[19:16] | size in words.
  const uint32_t header = CONTEXT_STATIC_BITS | (static_cast<uint32_t>(p.ctx_pkt_n & 0xfU) << 16) | CONTEXT_SIZE_WORDS;
  pack_u32(out + 0, header);

  // Offset 4: stream ID.
  pack_u32(out + 4, p.stream_id);

  // Offset 8: 8-byte class ID — OUI in upper 32-bit word, device class in lower.
  const uint64_t class_id = (static_cast<uint64_t>(OUI) << 32) | DEVICE_CLASS;
  pack_u64(out + 8, class_id);

  // Offset 16: timestamp — full seconds.
  pack_u32(out + 16, p.full_secs);

  // Offset 20: timestamp — fractional picoseconds (8 bytes).
  pack_u64(out + 20, p.frac_ps);

  // Offset 28: CIF0 = 0 (fixed DIFI context layout, no change indicators).
  pack_u32(out + 28, 0U);

  // Offset 32: reference point.
  pack_u32(out + 32, REF_POINT);

  // Offset 36: bandwidth (Hz × 2^20, unsigned) — set equal to sample rate.
  const auto bw_fixed = static_cast<uint64_t>(p.sample_rate_Hz * (1 << 20));
  pack_u64(out + 36, bw_fixed);

  // Offset 44: IF reference frequency (Hz × 2^20, signed) — 0 (baseband).
  pack_i64(out + 44, 0LL);

  // Offset 52: RF reference frequency (Hz × 2^20, signed).
  const auto rf_fixed = static_cast<int64_t>(p.center_freq_Hz * (1 << 20));
  pack_i64(out + 52, rf_fixed);

  // Offset 60: IF band offset (Hz × 2^20, signed) — 0.
  pack_i64(out + 60, 0LL);

  // Offset 68: reference level (4B; lower 16 bits = dBm × 128, signed int16) — 0.
  pack_u32(out + 68, 0U);

  // Offset 72: gains (4B; upper 16 = IF gain × 128, lower 16 = RF gain × 128).
  const auto rf_gain_fixed = static_cast<uint16_t>(static_cast<int16_t>(p.rf_gain_dB * 128.0));
  const auto if_gain_fixed = static_cast<uint16_t>(static_cast<int16_t>(p.if_gain_dB * 128.0));
  pack_u32(out + 72, (static_cast<uint32_t>(if_gain_fixed) << 16) | rf_gain_fixed);

  // Offset 76: sample rate (Hz × 2^20, unsigned).
  const auto sr_fixed = static_cast<uint64_t>(p.sample_rate_Hz * (1 << 20));
  pack_u64(out + 76, sr_fixed);

  // Offset 84: timestamp adjustment (femtoseconds).
  pack_u64(out + 84, TIMESTAMP_ADJ_FS);

  // Offset 92: timestamp calibration (unix seconds, mirrors full_secs).
  pack_u32(out + 92, p.full_secs);

  // Offset 96: state and event indicators.
  pack_u32(out + 96, DEFAULT_STATE_AND_EVENTS);

  // Offset 100: payload format — depends on bit depth.
  const uint64_t payload_fmt = (p.bit_depth == 8) ? PAYLOAD_FMT_8BIT : PAYLOAD_FMT_16BIT;
  pack_u64(out + 100, payload_fmt);
}
