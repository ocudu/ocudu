// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tx-chain bufferless LDPC codeblock segmentation.

#pragma once

#include "ldpc_graph_impl.h"
#include "ldpc_segmenter_helpers.h"
#include "ocudu/phy/upper/channel_coding/crc_calculator.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"

namespace ocudu {

/// \brief Bufferless implementation of Tx-chain LDPC segmentation.
class ldpc_segmenter_tx_impl : public ldpc_segmenter_tx, private ldpc_segmenter_buffer
{
public:
  /// CRC calculators used in shared channels.
  struct sch_crc {
    /// For short TB checksums.
    std::unique_ptr<crc_calculator> crc16;
    /// For long TB checksums.
    std::unique_ptr<crc_calculator> crc24A;
    /// For segment-specific checksums.
    std::unique_ptr<crc_calculator> crc24B;
  };

  /// \brief Initializes the internal Tx-chain LDPC segmenter blocks.
  /// \param[in] crc Unique pointers to CRC calculators.
  ldpc_segmenter_tx_impl(sch_crc& c) : crc_set({std::move(c.crc16), std::move(c.crc24A), std::move(c.crc24B)})
  {
    ocudu_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    ocudu_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    ocudu_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    ocudu_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16,
                 "Not a CRC generator of type CRC16.");
    ocudu_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A,
                 "Not a CRC generator of type CRC24A.");
    ocudu_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B,
                 "Not a CRC generator of type CRC24B.");
  }

  // See interface for the documentation.
  ldpc_segmenter_buffer& new_transmission(const segmenter_config& cfg) override;

private:
  /// Segmentation parameters.
  segment_parameters params;

  /// CRC calculators for transport-block and segment-specific checksums.
  sch_crc crc_set;

  // See ldpc_segmenter_buffer interface for documentation.
  void read_codeblock(bit_buffer codeblock, span<const uint8_t> transport_block, unsigned cb_index) const override;

  // See ldpc_segmenter_buffer interface for documentation.
  codeblock_metadata get_cb_metadata(unsigned cb_index) const override;

  // See ldpc_segmenter_buffer interface for documentation.
  unsigned get_nof_codeblocks() const override { return params.nof_segments; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_cb_info_bits(unsigned cb_index) const override;

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_segment_length() const override { return params.segment_length; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_cw_length() const override { return params.cw_length; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_zero_pad() const override { return params.zero_pad; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_tb_crc_bits() const override { return params.nof_tb_crc_bits; }

  // See ldpc_segmenter_buffer interface for documentation.
  units::bits get_nof_filler_bits() const override { return params.nof_filler_bits; }

  // See ldpc_segmenter_buffer interface for documentation.
  unsigned get_nof_short_segments() const override { return params.nof_short_segments; }

  // See ldpc_segmenter_buffer interface for documentation.
  unsigned get_rm_length(unsigned cb_index) const override;
};

} // namespace ocudu
