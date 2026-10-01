// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

namespace fapi_adaptor {
class phy_fapi_fastpath_adaptor;
} // namespace fapi_adaptor

class du_operation_controller;
class upper_phy_metrics_notifiers;

namespace odu {

class du_low;
class o_du_low_metrics_collector;

/// O-RAN DU low interface. O-DU low groups the DU low with the PHY-FAPI fastpath adaptor.
class o_du_low
{
public:
  /// Default destructor.
  virtual ~o_du_low() = default;

  /// Returns the operation controller of this DU low.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the upper PHY of this O-DU low.
  virtual du_low& get_du_low() = 0;

  /// Returns the PHY-FAPI fastpath adaptor of this O-DU low.
  virtual fapi_adaptor::phy_fapi_fastpath_adaptor& get_phy_fapi_fastpath_adaptor() = 0;

  /// Returns the metrics collector of this O-DU low.
  virtual o_du_low_metrics_collector* get_metrics_collector() = 0;
};

} // namespace odu
} // namespace ocudu
