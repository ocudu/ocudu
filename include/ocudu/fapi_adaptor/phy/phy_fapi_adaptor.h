// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace fapi_adaptor {

class phy_fapi_p5_sector_adaptor;
class phy_fapi_p7_sector_adaptor;

/// \brief PHY-FAPI bidirectional sector adaptor interface.
///
/// This adaptor represents a sector in FAPI, containing P5 (control/configuration) and P7 (user/slot) FAPI data planes.
class phy_fapi_sector_adaptor
{
public:
  virtual ~phy_fapi_sector_adaptor() = default;

  /// Returns the P5 sector adaptor of this PHY-FAPI sector adaptor.
  virtual phy_fapi_p5_sector_adaptor& get_p5_sector_adaptor() = 0;

  // :TODO: remove start/stop.
  virtual void start() = 0;
  virtual void stop()  = 0;

  /// Returns the P7 sector adaptor of this PHY-FAPI sector adaptor.
  virtual phy_fapi_p7_sector_adaptor& get_p7_sector_adaptor() = 0;
};

/// PHY-FAPI bidirectional adaptor interface.
class phy_fapi_adaptor
{
public:
  virtual ~phy_fapi_adaptor() = default;

  /// Returns the PHY-FAPI sector adaptor for the given cell id.
  virtual phy_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
