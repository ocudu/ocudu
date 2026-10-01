// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Generic hardware-accelerated PDSCH encoder functions declaration.

#pragma once

#include "ocudu/hal/hw_accelerator.h"
#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include "ocudu/phy/upper/channel_coding/crc_calculator.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_encoder.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"

namespace ocudu {

/// Generic hardware-accelerated implementation of the PDSCH encoder.
class pdsch_encoder_hw_impl : public pdsch_encoder
{
  static constexpr units::bytes CODEWORD_MAX_SIZE = pdsch_constants::CODEWORD_MAX_SIZE.round_up_to_bytes();

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

  /// \brief PDSCH encoder constructor.
  ///
  /// Sets up the internal components, namely LDPC segmenter, all the CRC calculators and the hardware accelerator.
  ///
  /// \param[in] c           Structure with pointers to three CRC calculator objects, with generator polynomials of type
  ///                        \c CRC16, \c CRC24A and \c CRC24B.
  /// \param[in] seg         Unique pointer to an LDPC segmenter.
  /// \param[in] hw          Unique pointer to a hardware-accelerator.
  pdsch_encoder_hw_impl(sch_crc&                                       c,
                        std::unique_ptr<ldpc_segmenter_tx>             seg,
                        std::unique_ptr<hal::hw_accelerator_pdsch_enc> hw) :
    crc_set({std::move(c.crc16), std::move(c.crc24A), std::move(c.crc24B)}),
    segmenter(std::move(seg)),
    encoder(std::move(hw))
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
    ocudu_assert(segmenter, "Invalid LDPC segmenter factory.");
    ocudu_assert(encoder, "Invalid hardware-accelerated PDSCH encoder.");
  }

  // See interface for the documentation.
  void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const configuration& config) override;

private:
  /// Defines if the PDSCH encoder operates in CB mode (true) or TB mode (false).
  bool cb_mode;

  /// Defines the maximum supported buffer size in bytes (CB mode will be forced for TBs requiring larger sizes). Only
  /// used in TB mode.
  unsigned max_buffer_size;

  /// CRC calculator for transport-block checksum.
  sch_crc crc_set;

  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;

  /// Pointer to a Hardware-accelerated PDSCH encoder.
  std::unique_ptr<hal::hw_accelerator_pdsch_enc> encoder;

  /// Pointer to an LDPC segmenter output buffer interface.
  const ldpc_segmenter_buffer* segment_buffer;

  /// \brief Temporary codeblock message.
  ///
  /// It contains codeblock information bits, codeblock CRC (if applicable) and filler bits.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_SIZE> cb_data;

  /// Buffer for storing temporary encoded and packed codeblock.
  static_vector<uint8_t, CODEWORD_MAX_SIZE.value()> codeblock_packed;

  /// \brief Computes the segmentation parameters required by the hardware-accelerated PDSCH encoder function.
  /// \param[out] hw_cfg          Hardware-accelerated PDSCH encoder configuration parameters.
  /// \param[in]  transport_block The transport block to encode (packed, one byte per entry).
  /// \param[in]  d_segs          Buffer storing the data segments comprising the transport block.
  /// \param[in]  cfg             PDSCH configuration parameters.
  void set_hw_enc_tb_configuration(hal::hw_pdsch_encoder_configuration& hw_cfg,
                                   span<const uint8_t>                  transport_block,
                                   const segmenter_config&              cfg);

  /// \brief Computes the segmentation parameters required by the hardware-accelerated PDSCH encoder function.
  /// \param[out] hw_cfg   Hardware-accelerated PDSCH encoder configuration parameters.
  /// \param[in]  cb_cfg   Optional. Code block PDSCH configuration parameters (CB mode only).
  /// \param[in]  cb_index Index of the CB (within the TB).
  void set_hw_enc_cb_configuration(hal::hw_pdsch_encoder_configuration& hw_cfg,
                                   const codeblock_metadata&            cb_cfg,
                                   unsigned                             cb_index);

  /// \brief Calculates the TB CRC checkshum from a byte buffer (8 packed bits in every input byte).
  /// \param[in] transport_block The transport block to encode (packed, one byte per entry).
  /// \return The resulting TB CRC checksum.
  crc_calculator_checksum_t compute_tb_crc(span<const uint8_t> transport_block, unsigned nof_tb_crc_bits);
};

} // namespace ocudu
