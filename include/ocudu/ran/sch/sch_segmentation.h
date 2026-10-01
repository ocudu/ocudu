// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/to_array.h"
#include "ocudu/ran/sch/ldpc_base_graph.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// Collects Shared Channel (SCH) parameters derived from the transport block size and the target code rate.
struct sch_information {
  /// Codeblock CRC size.
  static constexpr units::bits CB_CRC_SIZE{24};
  /// Transport block CRC size {16, 24}.
  units::bits tb_crc_size;
  /// LDPC base graph.
  ldpc_base_graph_type base_graph;
  /// Number of codeblocks {1, ..., 52}. Parameter \f$C\f$.
  unsigned nof_cb;
  /// Number of filler bits per codeblock. Parameter \f$F\f$.
  units::bits nof_filler_bits_per_cb;
  /// LDPC lifting size. Parameter \f$Z_c\f$.
  unsigned lifting_size;
  /// Number of bits per codeblock including filler bits. Parameter \f$K\f$.
  units::bits nof_bits_per_cb;
};

/// Gets the Shared Channel (SCH) derived parameters from the transport block size and target code rate, expressed as
/// \f$R\f$.
sch_information get_sch_segmentation_info(units::bits tbs, float target_code_rate);

/// \brief Computes the transport block CRC size from the transport block size.
///
/// \param[in] tbs Transport block size as a number of bits.
/// \return The number of CRC bits appended to a transport block.
constexpr units::bits compute_tb_crc_size(units::bits tbs)
{
  using namespace units::literals;
  constexpr units::bits MAX_BITS_CRC16 = 3824_bits;
  return ((tbs <= MAX_BITS_CRC16) ? 16_bits : 24_bits);
}

/// \brief Computes the number of codeblocks from a transport block size.
///
/// \param[in] tbs Transport block size as a number of bits (not including CRC).
/// \param[in] bg  Base graph.
/// \return The number of codeblocks a transport block of size \c tbs is encoded into when using using base graph \c bg.
constexpr unsigned compute_nof_codeblocks(units::bits tbs, ldpc_base_graph_type bg)
{
  using namespace units::literals;
  constexpr units::bits CBLOCK_CRC_LENGTH = 24_bits;
  units::bits           tb_and_crc_bits   = tbs + compute_tb_crc_size(tbs);

  units::bits max_segment_length = (bg == ldpc_base_graph_type::BG1) ? 8448_bits : 3840_bits;

  return ((tb_and_crc_bits <= max_segment_length)
              ? 1
              : divide_ceil(tb_and_crc_bits.value(), (max_segment_length - CBLOCK_CRC_LENGTH).value()));
}

/// \brief Computes the lifting size used to encode/decode a transport block.
///
/// As per TS38.212 Section 5.2.2, the lifting size depends on the size of the transport block, on the LDPC base graph
/// and on the number of segments the transport block is split into.
inline unsigned compute_lifting_size(units::bits tbs, ldpc_base_graph_type base_graph, unsigned nof_segments)
{
  /// Array of lifting sizes, for iterations.
  constexpr auto all_lifting_sizes =
      to_array<unsigned>({2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  18,  20,
                          22, 24,  26,  28,  30,  32,  36,  40,  44,  48,  52,  56,  60,  64,  72,  80,  88,
                          96, 104, 112, 120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384});

  using namespace units::literals;
  units::bits nof_tb_bits_in = tbs + compute_tb_crc_size(tbs);

  unsigned ref_length = 22;
  if (base_graph == ldpc_base_graph_type::BG2) {
    if (nof_tb_bits_in > 640_bits) {
      ref_length = 10;
    } else if (nof_tb_bits_in > 560_bits) {
      ref_length = 9;
    } else if (nof_tb_bits_in > 192_bits) {
      ref_length = 8;
    } else {
      ref_length = 6;
    }
  }

  unsigned total_ref_length = nof_segments * ref_length;

  // Calculate the number of bits per transport block including codeblock CRC.
  units::bits nof_tb_bits_out = nof_tb_bits_in;
  if (nof_segments > 1) {
    nof_tb_bits_out += units::bits(24 * nof_segments);
  }

  unsigned lifting_size = 0;
  for (auto ls : all_lifting_sizes) {
    if (ls * total_ref_length >= nof_tb_bits_out.value()) {
      lifting_size = ls;
      break;
    }
  }
  ocudu_assert(lifting_size != 0, "Lifting size cannot be 0");

  return lifting_size;
}

/// \brief Computes the codeblock size for the given base graph and lifting size.
///
/// As per TS38.212 Section 5.2.2, the number bits to encode is fixed for a given base graph and a given lifting size.
/// The codeblock consists of information bits, CRC bits and the number of filler bits required to match the codeblock
/// size.
constexpr units::bits compute_codeblock_size(ldpc_base_graph_type base_graph, unsigned lifting_size)
{
  constexpr unsigned base_length_BG1 = 22;
  constexpr unsigned base_length_BG2 = 10;
  unsigned           base_length     = (base_graph == ldpc_base_graph_type::BG1) ? base_length_BG1 : base_length_BG2;

  return units::bits(base_length * lifting_size);
}

} // namespace ocudu
