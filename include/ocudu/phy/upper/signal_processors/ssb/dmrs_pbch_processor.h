// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/ssb/ssb_configuration.h"

namespace ocudu {

class resource_grid_writer;

/// Describes a DMRS for PBCH processor interface.
class dmrs_pbch_processor
{
public:
  /// Describes the required parameters to generate the signal.
  struct config_t {
    /// Physical cell identifier.
    pci_t phys_cell_id;
    /// SS/PBCH block index in the burst.
    ssb_id_t ssb_idx;
    /// Maximum number of SS/PBCH transmissions in a burst (5ms).
    unsigned L_max;
    /// First subcarrier in the resource grid.
    unsigned ssb_first_subcarrier;
    /// First symbol of the SS/PBCH block within the slot.
    unsigned ssb_first_symbol;
    /// Flag: true if the SS/PBCH block transmission is in an odd half frame, false otherwise.
    bool hrf;
    /// Linear signal amplitude.
    float amplitude;
    /// Port indexes to map the signal.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pbch_processor() = default;

  /// \brief Generates and maps DM-RS for PBCH.
  /// \param[out] grid Destination resource grid.
  /// \param[in]  config Configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace ocudu
