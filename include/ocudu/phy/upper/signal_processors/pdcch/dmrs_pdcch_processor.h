// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/precoding_beamforming_configuration.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

class resource_grid_mapper;

/// Describes a DMRS for PDCCH processor interface.
class dmrs_pdcch_processor
{
public:
  /// Describes the necessary parameters to generate DMRS for a PDCCH transmission.
  struct config_t {
    /// Provides the slot timing and numerology.
    slot_point slot;
    /// Cyclic prefix.
    cyclic_prefix cp;
    /// Reference point for PDCCH DMRS \e k in RBs.
    unsigned reference_point_k_rb;
    /// Indicates the PRB used for the PDCCH transmission. The bit at position zero corresponds to CRB0.
    crb_bitmap rb_mask;
    /// CORESET start symbol index.
    unsigned start_symbol_index;
    /// CORESET duration in symbols.
    unsigned duration;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Provides the linear signal amplitude to conform with the transmission power.
    float amplitude;
    /// Precoding and beamforming configuration.
    precoding_beamforming_configuration precoding_and_beamforming;
  };

  /// Default destructor.
  virtual ~dmrs_pdcch_processor() = default;

  /// \brief Generates and maps DMRS for PDCCH according to TS38.211 Section 7.4.1.3.
  ///
  /// \param[out] grid   Resource grid writer interface.
  /// \param[in] config  Required configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace ocudu
