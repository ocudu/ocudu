// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/precoding_beamforming_configuration.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"

namespace ocudu {

class resource_grid_writer;

/// \brief Describes a PDCCH modulator interface.
///
/// \remark Defined in TS38.211 Section 7.3.2 Physical downlink control channel (PDCCH).
/// \note The configuration assumes the CCE-to-REG and REG-to-PRB mapping is included in \c rb_mask.
class pdcch_modulator
{
public:
  /// Describes the necessary parameters to modulate a PDCCH transmission.
  struct config_t {
    /// Indicates the RBs used for the PDCCH transmission.
    crb_bitmap rb_mask;
    /// CORESET start symbol index.
    unsigned start_symbol_index;
    /// CORESET duration in symbols.
    unsigned duration;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling according to TS38.211 Section 7.3.2.3.
    unsigned n_rnti;
    /// Scaling factor to apply to the resource elements according to PDCCH power allocation in TS38.213.
    float scaling;
    /// Precoding and beamforming configuration.
    precoding_beamforming_configuration precoding_and_beamforming;
  };

  /// Default destructor.
  virtual ~pdcch_modulator() = default;

  /// \brief Modulates a PDCCH codeword according to TS38.211 Section 7.3.2.
  /// \param[out] grid   Resource grid writer interface.
  /// \param[in] data    PDCCH codeword to modulate.
  /// \param[in] config  Necessary parameters for modulating PDCCH.
  /// \remark The codeword length shall be consistent with the resource mapping.
  /// \remark An assertion is triggered if the number of precoding layers is not one.
  virtual void modulate(resource_grid_writer& grid, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace ocudu
