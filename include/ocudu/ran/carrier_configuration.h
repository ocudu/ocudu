// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "arfcn.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/bs_channel_bandwidth.h"
#include "ocudu/ran/nr_band.h"
#include <cstdint>

namespace ocudu {

/// \brief Configuration of each transmission point associated to the corresponding cell(s). This includes
/// different physical antennas, different frequencies, bandwidths.
struct carrier_configuration {
  /// Width of this carrier in MHz.
  bs_channel_bandwidth carrier_bw = bs_channel_bandwidth::MHz10;
  /// NR Absolute Radio Frequency Channel Number (NR-ARFCN) of "F_REF", which is the RF reference frequency, as per
  /// TS 38.104, Section 5.4.2.1 ("F_REF" maps to the central frequency of the band).
  arfcn_t arfcn_f_ref = 365000;
  /// <em>NR operating band<\em>, as per Table 5.2-1 and 5.2-2. TS 38.104.
  nr_band band = nr_band::invalid;
  /// Number of antennas. Values: (0..65355).
  uint16_t nof_ant = 1;
  /// Topology of the antennas.
  antenna_topology topology = antenna_topology::one_port;
};

} // namespace ocudu
