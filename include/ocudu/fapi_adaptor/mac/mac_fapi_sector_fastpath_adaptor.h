// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace fapi_adaptor {

class mac_fapi_p5_sector_fastpath_adaptor;
class mac_fapi_p7_sector_fastpath_adaptor;
class operation_controller;

/// \brief MAC-FAPI bidirectional sector fastpath adaptor interface.
///
/// This adaptor represents a sector in FAPI, containing the operation controller of the sector, P5
/// (control/configuration) and P7 (user/slot) FAPI data planes.
class mac_fapi_sector_fastpath_adaptor
{
public:
  virtual ~mac_fapi_sector_fastpath_adaptor() = default;

  /// Returns the operation controller of this sector adaptor.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the P5 adaptor of this sector fastpath adaptor.
  virtual mac_fapi_p5_sector_fastpath_adaptor& get_p5_sector_fastpath_adaptor() = 0;

  /// Returns the P7 adaptor of this sector fastpath adaptor.
  virtual mac_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
