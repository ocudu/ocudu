// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PDSCH encoder declaration.

#pragma once

#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_encoder.h"
#include "ocudu/phy/upper/codeblock_metadata.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"

namespace ocudu {

/// Generic implementation of the PDSCH encoder.
class pdsch_encoder_impl : public pdsch_encoder
{
public:
  /// \brief Initializes the internal LDPC blocks.
  /// \param[in] seg Unique pointer to an LDPC segmenter.
  /// \param[in] enc Unique pointer to an LDPC encoder.
  /// \param[in] rm  Unique pointer to an LDPC rate matcher.
  pdsch_encoder_impl(std::unique_ptr<ldpc_segmenter_tx> seg,
                     std::unique_ptr<ldpc_encoder>      enc,
                     std::unique_ptr<ldpc_rate_matcher> rm) :
    segmenter(std::move(seg)), encoder(std::move(enc)), rate_matcher(std::move(rm))
  {
    ocudu_assert(segmenter, "Invalid LDPC segmenter.");
    ocudu_assert(encoder, "Invalid LDPC encoder.");
    ocudu_assert(rate_matcher, "Invalid LDPC rate matcher.");
  }

  // See interface for the documentation.
  void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const configuration& config) override;

private:
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_tx> segmenter;
  /// Pointer to an LDPC encoder.
  std::unique_ptr<ldpc_encoder> encoder;
  /// Pointer to an LDPC rate matcher.
  std::unique_ptr<ldpc_rate_matcher> rate_matcher;

  /// \brief Temporary codeblock message.
  ///
  /// It contains codeblock information bits, codeblock CRC (if applicable) and filler bits.
  static_bit_buffer<ldpc::MAX_CODEBLOCK_SIZE> cb_data;
  /// \brief Maximum codeblock length.
  ///
  /// This is the maximum length of an encoded codeblock, achievable with base graph 1 (rate 1/3).
  static constexpr units::bits MAX_CB_LENGTH{3 * MAX_SEG_LENGTH.value()};
  /// Buffer for storing temporary encoded and packed codeblock.
  static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()> codeblock_packed;
};

} // namespace ocudu
