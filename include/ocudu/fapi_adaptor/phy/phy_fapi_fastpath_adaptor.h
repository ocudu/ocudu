// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace fapi_adaptor {

class phy_fapi_sector_fastpath_adaptor;

/// PHY-FAPI bidirectional adaptor interface.
class phy_fapi_fastpath_adaptor
{
public:
  virtual ~phy_fapi_fastpath_adaptor() = default;

  /// Returns the PHY-FAPI sector fastpath adaptor for the given cell id.
  virtual phy_fapi_sector_fastpath_adaptor& get_sector_adaptor(unsigned cell_id) = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
