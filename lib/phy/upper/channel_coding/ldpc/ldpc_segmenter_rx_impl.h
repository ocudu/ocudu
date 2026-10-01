// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC codeblock segmentation declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "ldpc_segmenter_helpers.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"

namespace ocudu {

/// \brief Generic implementation of Rx-chain LDPC segmentation.
class ldpc_segmenter_rx_impl : public ldpc_segmenter_rx
{
public:
  /// Default constructor.
  ldpc_segmenter_rx_impl() = default;

  // See interface for the documentation.
  void segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
               span<const log_likelihood_ratio>                         codeword_llrs,
               const segmenter_config&                                  cfg) override;

private:
  /// Segmentation parameters.
  segment_parameters params;
};

} // namespace ocudu
