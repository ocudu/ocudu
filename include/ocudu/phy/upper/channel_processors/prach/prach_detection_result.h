// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/prach/prach_constants.h"

namespace ocudu {

/// Describes a PRACH detection result.
struct prach_detection_result {
  /// Describes the detection of a single preamble.
  struct preamble_indication {
    /// Index of the detected preamble. Possible values are {0, ..., 63}.
    unsigned preamble_index;
    /// Timing advance between the observed arrival time (for the considered UE) and the reference uplink time.
    phy_time_unit time_advance;
    /// Detection metric normalized with respect to the detection threshold.
    float detection_metric;
    /// Preamble received power in normalized dB units.
    float preamble_power_dB;
  };

  /// Average RSSI value in normalized dB units.
  float rssi_dB;
  /// \brief Detector time resolution.
  ///
  /// This is equal to the PRACH subcarrier spacing divided by the DFT size of the detector.
  phy_time_unit time_resolution;
  /// \brief Detector maximum time in advance.
  ///
  /// This is equal to the minimum value among \f$N_{CP}^{RA}\f$ and \f$N_{CS}\f$ if \f$N_{CS}\f$ is not zero.
  /// Otherwise, it is equal to \f$N_{CP}^{RA}\f$.
  phy_time_unit time_advance_max;
  /// List of detected preambles.
  static_vector<preamble_indication, prach_constants::MAX_NUM_PREAMBLES> preambles;
};

} // namespace ocudu
