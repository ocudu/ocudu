// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_data_packet.h"
#include "radio_difi_packing.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/ocudu_assert.h"
#include <cstring>

using namespace ocudu;

/// Header template: type=0x1 (IF data), class-id present, POSIX timestamp mode.
static constexpr uint32_t DATA_STATIC_BITS = 0x18e00000U;
/// DIFI Organizational Unique Identifier — written into the class ID field so
/// Wireshark's DIFI dissector can identify data packets by the same OUI used
/// in context packets.
static constexpr uint64_t DATA_CLASS_ID = 0x6a621eULL << 32;

/// Number of picoseconds in one second — the DIFI fractional timestamp unit.
static constexpr uint64_t PICO_PER_SEC = 1000000000000ULL;
/// Largest packet the VRT header can describe: the size field is 16 bits wide and counts 32-bit words.
static constexpr uint32_t MAX_PACKET_SIZE_WORDS = 0xffffU;

///
/// Public API.
///

void ocudu::difi_ticks_to_time(uint64_t ticks, double sample_rate_Hz, uint32_t& full_secs, uint64_t& frac_ps)
{
  if (sample_rate_Hz <= 0.0) {
    full_secs = 0;
    frac_ps   = 0;
    return;
  }

  const uint64_t srate = static_cast<uint64_t>(sample_rate_Hz);
  const uint64_t rem   = ticks % srate;

  full_secs = static_cast<uint32_t>(ticks / srate);

  // frac_ps = rem * PICO_PER_SEC / srate, split into quotient and remainder of srate because a direct
  // multiplication reaches 1e20, beyond the range of uint64_t. Exact, since
  // rem * PICO_PER_SEC == rem * q * srate + rem * r.
  const uint64_t q = PICO_PER_SEC / srate;
  const uint64_t r = PICO_PER_SEC % srate;
  frac_ps          = rem * q + (rem * r) / srate;
}

uint64_t ocudu::difi_time_to_ticks(uint32_t full_secs, uint64_t frac_ps, double sample_rate_Hz)
{
  if (sample_rate_Hz <= 0.0) {
    return 0;
  }

  // Whole seconds are exact integers; only the sub-second quotient goes through floating point, and it
  // never exceeds the sample rate. Rounding rather than truncating makes difi_ticks_to_time() round-trip
  // to the same tick, since a one-tick error would look like a lost sample.
  const double frac_ticks = static_cast<double>(frac_ps) * sample_rate_Hz / static_cast<double>(PICO_PER_SEC);

  return static_cast<uint64_t>(full_secs) * static_cast<uint64_t>(sample_rate_Hz) +
         static_cast<uint64_t>(frac_ticks + 0.5);
}

units::bytes ocudu::difi_data_packet_size(unsigned bit_depth, unsigned nof_samples)
{
  const unsigned payload_bytes = (bit_depth == 8) ? nof_samples * 2U : nof_samples * 4U;
  // Pad to 4-byte (32-bit word) boundary.
  const unsigned payload_padded = (payload_bytes + 3U) & ~3U;
  return DIFI_DATA_HEADER_SIZE + units::bytes(payload_padded);
}

units::bytes
ocudu::build_difi_data_packet(span<uint8_t> buf, const difi_data_packet_params& p, span<const ci16_t> samples)
{
  const units::bytes total_bytes = difi_data_packet_size(p.bit_depth, samples.size());
  // Words rather than bytes, so the count leaves the units type here.
  const uint32_t total_words = total_bytes.value() / 4;

  // The size field is only 16 bits wide; a larger count would silently overwrite the packet type and
  // sequence number in the header word.
  ocudu_assert(total_words <= MAX_PACKET_SIZE_WORDS,
               "DIFI data packet of '{}' words from '{}' samples exceeds the '{}' word maximum",
               total_words,
               samples.size(),
               MAX_PACKET_SIZE_WORDS);
  ocudu_assert(buf.size() >= total_bytes.value(),
               "Buffer of '{}' bytes is too small for a DIFI data packet of '{}' bytes",
               buf.size(),
               total_bytes.value());

  uint8_t* out = buf.data();

  // Zero the entire buffer so any padding bytes are already clean.
  ocuduvec::zero(buf.first(total_bytes.value()));

  // Offset 0: header word — static bits | pkt_n in bits[19:16] | size in words.
  const uint32_t header = DATA_STATIC_BITS | (static_cast<uint32_t>(p.pkt_n & 0xfU) << 16) | total_words;
  pack_u32(out + 0, header);

  // Offset 4: stream ID.
  pack_u32(out + 4, p.stream_id);

  // Offset 8: class ID — DIFI OUI (0x6a621e) in upper 32-bit word, device class in lower.
  pack_u64(out + 8, DATA_CLASS_ID);

  // Offset 16: timestamp — full seconds.
  pack_u32(out + 16, p.full_secs);

  // Offset 20: timestamp — fractional picoseconds (8 bytes).
  pack_u64(out + 20, p.frac_ps);

  // Offset 28: IQ payload, in host byte order while every metadata field above is big endian. That
  // asymmetry is the DIFI convention: the Consortium's certification transmitter serialises the
  // payload natively while byte-swapping the header.
  uint8_t* dst = out + DIFI_DATA_HEADER_SIZE.value();

  if (p.bit_depth == 16) {
    for (const ci16_t s : samples) {
      const int16_t re = s.real();
      const int16_t im = s.imag();
      std::memcpy(dst, &re, 2);
      dst += 2;
      std::memcpy(dst, &im, 2);
      dst += 2;
    }
  } else {
    // 8-bit: upper byte of each int16 component is the int8 sample value.
    for (const ci16_t s : samples) {
      *dst++ = static_cast<uint8_t>(static_cast<int8_t>(s.real() >> 8));
      *dst++ = static_cast<uint8_t>(static_cast<int8_t>(s.imag() >> 8));
    }
    // Padding bytes are already zero from the buffer zeroing above.
  }

  return total_bytes;
}
