// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/support/units.h"
#include <cstdint>

namespace ocudu {

/// Fixed size of a DIFI standard context packet in bytes (27 × 4).
constexpr units::bytes DIFI_CONTEXT_PACKET_SIZE{108};

/// Parameters required to build a DIFI standard context packet.
struct difi_context_packet_params {
  /// DIFI 32-bit stream identifier.
  uint32_t stream_id;
  /// Timestamp: whole seconds since the Unix epoch.
  uint32_t full_secs;
  /// Timestamp: sub-second part in picoseconds [0, 1e12).
  uint64_t frac_ps;
  /// Sample rate in Hz — encoded into the sample_rate and bandwidth fields.
  double sample_rate_Hz;
  /// RF centre frequency in Hz — encoded into the rf_ref_freq field.
  double center_freq_Hz;
  /// IQ sample bit depth (8 or 16) — selects the payload_format constant.
  unsigned bit_depth;
  /// RF gain in dB — encoded into the lower 16 bits of the gains field.
  double rf_gain_dB;
  /// IF gain in dB — encoded into the upper 16 bits of the gains field.
  double if_gain_dB;
  /// Context packet sequence number (0–15, mod-16 counter).
  uint8_t ctx_pkt_n;
};

/// \brief Fills \p buf with a 108-byte DIFI standard context packet, all fields big endian.
///
/// \p buf must hold at least DIFI_CONTEXT_PACKET_SIZE bytes; a shorter span trips an assertion.
void build_difi_context_packet(span<uint8_t> buf, const difi_context_packet_params& p);

} // namespace ocudu
