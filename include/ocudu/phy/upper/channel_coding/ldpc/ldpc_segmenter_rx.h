// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC codeblock segmentation (receive side).
#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/upper/codeblock_metadata.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"

namespace ocudu {

/// Carries out the segmentation of a codeword into a number of codeblocks.
class ldpc_segmenter_rx
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter_rx() = default;

  /// \brief Splits a codeword into codeblocks and computes the metadata of all codeblocks.
  ///
  /// The LLRs corresponding to the received codeword are split into codeblocks to feed the decoder. Each codeblock is
  /// described by all the metadata needed for rate-dematching and decoding, as per TS38.212 Section 5.4.2.1.
  /// \param[out] described_codeblocks  Codeblock LLRs and corresponding metadata.
  /// \param[in]  codeword_llrs         Log-likelihood ratios of the received codeword.
  /// \param[in]  cfg                   Parameters affecting splitting and codeblock metadata.
  /// \remark The output codeblocks are just views (not copies) of the proper block of codeword log-likelihood ratios in
  /// \c codeword_llrs.
  virtual void segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                       span<const log_likelihood_ratio>                         codeword_llrs,
                       const segmenter_config&                                  cfg) = 0;
};

} // namespace ocudu
