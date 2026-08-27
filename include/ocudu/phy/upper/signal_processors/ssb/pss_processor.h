// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pci.h"
#include "ocudu/ran/precoding_beamforming_configuration.h"
#include <memory>

namespace ocudu {

class resource_grid_writer;

/// Describes a PSS processor interface
class pss_processor
{
public:
  /// Describes the required parameters to generate the signal
  struct config_t {
    /// Physical cell identifier
    pci_t phys_cell_id;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// Denotes the first symbol of the SS/PBCH block within the slot.
    unsigned ssb_first_symbol;
    /// PSS linear signal amplitude
    float amplitude;
    /// Precoding and beamforming configuration.
    precoding_beamforming_configuration precoding_and_beamforming;
  };

  /// Default destructor
  virtual ~pss_processor() = default;

  /// \brief Generates and maps a PSS sequence
  /// \param [out] grid Provides the destination resource grid
  /// \param [in] config Provides the required configuration to generate and map the signal
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace ocudu
