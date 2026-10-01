// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/phy_time_unit.h"
#include <chrono>

namespace ocudu {

/// Lower physical layer transmit time offset controller interface.
class lower_phy_tx_time_offset_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_tx_time_offset_controller() = default;

  /// \brief Sets a transmit time offset.
  virtual void set_tx_time_offset(phy_time_unit tx_time_offset) = 0;
};

} // namespace ocudu
