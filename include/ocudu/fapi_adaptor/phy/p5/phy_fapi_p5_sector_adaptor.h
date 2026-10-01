// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

namespace fapi {
class error_indication_notifier;
class p5_requests_gateway;
class p5_responses_notifier;
} // namespace fapi

namespace fapi_adaptor {

/// \brief: PHY-FAPI P5 sector adaptor.
///
/// The PHY-FAPI P5 sector adaptor handles FAPI P5 messages that allows to configure, start or stop a PHY.
///
/// Note: This interface is used to integrate a third-party L1.
class phy_fapi_p5_sector_adaptor
{
public:
  virtual ~phy_fapi_p5_sector_adaptor() = default;

  /// Returns the P5 requests message gateway of this FAPI cell configurator.
  virtual fapi::p5_requests_gateway& get_p5_requests_gateway() = 0;

  /// Sets the P5 responses notifier of this FAPI cell configurator.
  virtual void set_p5_responses_notifier(fapi::p5_responses_notifier& config_notifier) = 0;

  /// Sets the error indication notifier of this FAPI cell configurator.
  virtual void set_error_indication_notifier(fapi::error_indication_notifier& err_notifier) = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
