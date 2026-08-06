// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/support/units.h"
#include <cstdint>

namespace ocudu {

/// Fixed size of the DIFI data packet header in bytes.
constexpr units::bytes DIFI_DATA_HEADER_SIZE{28};

/// Parameters for the fixed DIFI data packet header fields.
struct difi_data_packet_params {
  /// Stream identifier.
  uint32_t stream_id;
  /// Timestamp whole seconds.
  uint32_t full_secs;
  /// Timestamp sub-second part in picoseconds.
  uint64_t frac_ps;
  /// IQ sample bit depth, either 8 or 16.
  unsigned bit_depth;
  /// Data packet sequence number, a mod-16 counter.
  uint8_t pkt_n;
};

/// \brief Writes a DIFI data packet into \p buf and returns the total byte count.
///
/// A 28-byte big-endian header, then interleaved IQ in host byte order, zero-padded to a 32-bit word.
///
/// \p buf must hold at least difi_data_packet_size() bytes, and the packet must not exceed the 65535-word
/// maximum the VRT size field can express; either violation trips an assertion.
units::bytes build_difi_data_packet(span<uint8_t> buf, const difi_data_packet_params& p, span<const ci16_t> samples);

/// Returns the packet byte count for \p nof_samples, always a multiple of 4.
units::bytes difi_data_packet_size(unsigned bit_depth, unsigned nof_samples);

/// \brief Converts a sample-tick timestamp into the DIFI whole seconds and picoseconds pair.
///
/// Integer arithmetic: a double loses precision at Unix-epoch tick counts, costing several samples.
void difi_ticks_to_time(uint64_t ticks, double sample_rate_Hz, uint32_t& full_secs, uint64_t& frac_ps);

/// Inverse of difi_ticks_to_time(), round-tripping its output exactly.
uint64_t difi_time_to_ticks(uint32_t full_secs, uint64_t frac_ps, double sample_rate_Hz);

} // namespace ocudu
