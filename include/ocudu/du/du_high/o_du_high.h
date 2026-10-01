// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class du_operation_controller;

namespace fapi_adaptor {
class mac_fapi_fastpath_adaptor;
} // namespace fapi_adaptor

namespace odu {

class du_high;
class o_du_high_metrics_notifier;

/// O-RAN Distributed Unit high interface. O-DU high groups the DU high with the MAC-FAPI adaptor.
class o_du_high
{
public:
  virtual ~o_du_high() = default;

  /// Returns the operation controller of this O-RAN DU high.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the DU high from this O-RAN DU high.
  virtual du_high& get_du_high() = 0;

  /// Returns the MAC-FAPI fastpath adaptor of this O-RAN DU high.
  virtual fapi_adaptor::mac_fapi_fastpath_adaptor& get_mac_fapi_fastpath_adaptor() = 0;

  /// Sets the O-DU high metrics notifier to the given one.
  virtual void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier) = 0;
};

} // namespace odu
} // namespace ocudu
