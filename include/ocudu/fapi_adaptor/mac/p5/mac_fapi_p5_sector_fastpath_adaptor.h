// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_base_adaptor.h"

namespace ocudu {

class phy_cell_operation_controller;

namespace fapi_adaptor {

/// \brief MAC-FAPI P5 sector fastpath adaptor interface.
///
/// The fastpath adaptor is used by the MAC layer to start/stop the cell.
class mac_fapi_p5_sector_fastpath_adaptor : public mac_fapi_p5_sector_base_adaptor
{
public:
  /// Returns the PHY cell operation controller of this adaptor.
  virtual phy_cell_operation_controller& get_operation_controller() = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
