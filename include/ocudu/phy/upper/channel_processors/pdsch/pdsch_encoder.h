// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PDSCH encoder interface.
///
/// PDSCH encoder black-box interface, with a single method that takes a transport block as input and returns a
/// codeword.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/codeblock_metadata.h"

namespace ocudu {

class tx_buffer;

/// \brief PDSCH encoder, as per TS38.212 Section 7.2.
///
/// For a PHY Downlink Shared Data CHannel (PDSCH) transport block, it returns the single codeword obtained after CRC
/// attachment, segmentation, codeblock encoding, rate matching and, finally, codeblock concatenation.
class pdsch_encoder
{
public:
  /// Default destructor.
  virtual ~pdsch_encoder() = default;

  /// Collects PDSCH encoder parameters.
  struct configuration {
    /// Code base graph.
    ldpc_base_graph_type base_graph;
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv;
    /// Modulation scheme.
    modulation_scheme mod;
    /// \brief Limited buffer rate matching length in bits, as per TS38.212 Section 5.4.2.
    /// \note Set to zero for unlimited buffer length.
    unsigned Nref;
    /// Number of transmission layers the transport block is mapped onto.
    unsigned nof_layers;
    /// Number of channel symbols (i.e., REs) the transport block is mapped to.
    unsigned nof_ch_symbols;
  };

  /// \brief Full PDSCH encoding.
  ///
  /// General encoding method that includes the four steps of CRC attachment (see TS38.212 Section 7.2.1), transport
  /// block segmentation (see TS38.212 Section 7.2.3), codeblock encoding (see TS38.212 Section 7.2.4) and rate matching
  /// (see TS38.212 Section 7.2.5), and codeblock concatenation (see TS38.212 Section 7.2.6) into the PDSCH codeword.
  /// \param[out]    codeword         Final PDSCH codeword.
  /// \param[in]     transport_block  Transport block to be transmitted.
  /// \param[in]     cfg              PDSCH configuration parameters.
  virtual void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const configuration& cfg) = 0;
};

} // namespace ocudu
