// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

namespace fapi {
class error_indication_notifier;
class p7_indications_notifier;
class p7_last_request_notifier;
class p7_requests_gateway;
class p7_slot_indication_notifier;
} // namespace fapi

namespace fapi_adaptor {

/// \brief PHY-FAPI bidirectional P7 sector adaptor interface.
///
/// This adaptor is a collection of interfaces to connect a P7 MAC-FAPI sector adaptor to a PHY-FAPI P7 sector adaptor.
///
/// Note: This interface is used to integrate a third-party L1.
class phy_fapi_p7_sector_adaptor
{
public:
  virtual ~phy_fapi_p7_sector_adaptor() = default;

  /// Returns a reference to the P7 requests gateway used by the adaptor.
  virtual fapi::p7_requests_gateway& get_p7_requests_gateway() = 0;

  /// Returns a reference to the P7 last request notifier used by the adaptor.
  virtual fapi::p7_last_request_notifier& get_p7_last_request_notifier() = 0;

  /// Configures the P7 slot indication notifier to the given one.
  virtual void set_p7_slot_indication_notifier(fapi::p7_slot_indication_notifier& notifier) = 0;

  /// Configures the error indication notifier to the given one.
  virtual void set_error_indication_notifier(fapi::error_indication_notifier& fapi_error_notifier) = 0;

  /// Configures the P7 slot indications notifier to the given one.
  virtual void set_p7_indications_notifier(fapi::p7_indications_notifier& fapi_notifier) = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
