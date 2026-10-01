// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief LDPC codeblock segmentation (transmit side).
#pragma once

#include "ocudu/phy/upper/codeblock_metadata.h"

namespace ocudu {

class ldpc_segmenter_buffer;

/// Carries out the segmentation of a transport block into a number of codeblocks.
class ldpc_segmenter_tx
{
public:
  /// Default destructor.
  virtual ~ldpc_segmenter_tx() = default;

  /// \brief Configures a new transport block for performing the segmentation on a codeblock basis without intermediate
  /// buffers.
  ///
  /// If needed, a CRC is attached to the generated segment, according to TS38.212 Section 5.2.2. Moreover, the
  /// transport block CRC is attached to the last segment, as per TS38.212 Section 7.2.1. The function also computes
  /// relevant segment metadata (e.g., coded and rate-matched lengths) according to TS38.212 Section 5.4.2.1.
  ///
  /// \param[in] cfg Parameters affecting splitting and codeblock metadata.
  /// \return A reference to the LDPC segmenter buffer.
  virtual const ldpc_segmenter_buffer& new_transmission(const segmenter_config& cfg) = 0;
};

} // namespace ocudu
