// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/format.h"

namespace ocudu {

/// Labels that code how many bits are used for SR.
/// For PUCCH Format 0-1, valid values: no_sr = no SR occasion; one = SR occasion.
/// For PUCCH Format 2-3-4, all possible values are valid.
enum class sr_nof_bits : unsigned { no_sr = 0, one, two, three, four };

/// Converts \ref sr_nof_bits into unsigned.
inline unsigned sr_nof_bits_to_uint(sr_nof_bits sr_bits)
{
  return static_cast<unsigned>(sr_bits);
}

/// Implements the + operator for \ref sr_nof_bits.
inline sr_nof_bits operator+(sr_nof_bits x, sr_nof_bits y)
{
  const unsigned sum = sr_nof_bits_to_uint(x) + sr_nof_bits_to_uint(y);
  return sum > sr_nof_bits_to_uint(sr_nof_bits::four) ? sr_nof_bits::four : static_cast<sr_nof_bits>(sum);
}

/// Contains the number of UCI bits (HARQ-ACK, SR and CSI) of a PUCCH grant.
struct pucch_uci_bits {
  /// Number of HARQ-ACK info bits that should have been reported in the removed PUCCH grant.
  unsigned harq_ack_nof_bits{0};
  /// Number of SR info bits that should have been reported in the removed PUCCH grant.
  sr_nof_bits sr_bits{sr_nof_bits::no_sr};
  /// Number of CSI Part 1 info bits that should have been reported in the removed PUCCH grant.
  unsigned csi_part1_nof_bits{0};
  // TODO: add extra bits for CSI Part 2.

  [[nodiscard]] unsigned get_total_bits() const
  {
    return harq_ack_nof_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_nof_bits;
  }

  bool operator==(const pucch_uci_bits& other) const
  {
    return harq_ack_nof_bits == other.harq_ack_nof_bits and sr_bits == other.sr_bits and
           csi_part1_nof_bits == other.csi_part1_nof_bits;
  }
  bool operator!=(const pucch_uci_bits& other) const { return not(*this == other); }
};

} // namespace ocudu

// Formatters.
namespace fmt {

template <>
struct formatter<ocudu::pucch_uci_bits> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::pucch_uci_bits& uci_bits, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "uci_bits=[H={} S={} C={}]",
                     uci_bits.harq_ack_nof_bits,
                     fmt::underlying(uci_bits.sr_bits),
                     uci_bits.csi_part1_nof_bits);
  }
};

} // namespace fmt
