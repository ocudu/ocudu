// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/sch/ldpc_base_graph.h"
#include "ocudu/ran/sch/modulation_scheme.h"

namespace ocudu {

class ldpc_segmenter_buffer;

/// \brief Describes the PDSCH block processor interface.
///
/// The PDSCH block processor carries out CRC attachment, LDPC encoding, rate matching, scrambling, and modulation of a
/// contiguous number of codeblocks within a transmission.
class pdsch_block_processor
{
public:
  /// Default destructor.
  virtual ~pdsch_block_processor() = default;

  /// PDSCH block processing parameters.
  struct configuration {
    /// Parameter \f$n_{RNTI}\f$ from TS38.211 Section 7.3.1.1 Scrambling.
    rnti_t rnti;
    /// Modulation scheme.
    modulation_scheme modulation;
    /// Redundancy version index.
    unsigned rv;
    /// Parameter \f$n_{ID}\f$ from TS38.211 Section 7.3.1.1.
    unsigned n_id;
    /// Parameter \f$N^{n_{SCID}}_{ID}\f$ TS38.211 Section 7.4.1.1.1.
    unsigned scrambling_id;
    /// LDPC base graph to use for CW generation.
    ldpc_base_graph_type ldpc_base_graph;
    /// Number of resource elements the PDSCH transmission is effectively mapped onto.
    unsigned nof_re_pdsch;
    /// Limited buffer rate matching length, as per TS38.212 Section 5.4.2. Set to zero for unlimited buffer length.
    units::bits Nref;
    /// Number of layers the codeword is mapped onto.
    unsigned nof_layers;
  };

  /// \brief Configures a new transmission.
  ///
  /// The method configure_new_transmission() configures the processor for a new transmission and returns a reference to
  /// a resource grid mapper buffer interface \ref resource_grid_mapper::symbol_buffer. After that, the processor will
  /// process codeblocks as the resource grid mapper requests resource elements to map into the resource grid.
  ///
  /// \param[in] data         Transport block data.
  /// \param[in] i_cw         Codeword index.
  /// \param[in] config       PDSCH transmission block processing parameters.
  /// \param[in] start_i_cb   Index of the first CB in the batch.
  /// \param[in] cb_batch_len Length of the CB batch.
  /// \return A reference to the complex symbol buffer of the resource element mapping interface.
  virtual resource_grid_mapper::symbol_buffer& configure_new_transmission(span<const uint8_t>          data,
                                                                          unsigned                     i_cw,
                                                                          const configuration&         config,
                                                                          const ldpc_segmenter_buffer& segment_buffer,
                                                                          unsigned                     start_i_cb,
                                                                          unsigned cb_batch_len) = 0;
};

} // namespace ocudu
