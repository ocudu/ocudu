// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Lower physical layer carrier center frequency controller interface.
class lower_phy_center_freq_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_center_freq_controller() = default;

  /// Sets the carrier center frequency to the given value. Returns true on success, otherwise false.
  virtual bool set_carrier_center_frequency(double carrier_center_frequency_Hz) = 0;
};

} // namespace ocudu
