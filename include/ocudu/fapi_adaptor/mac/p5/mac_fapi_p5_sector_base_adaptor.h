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

/// \brief MAC-FAPI P5 sector base adaptor interface.
///
/// This adaptor manages the FAPI P5 messages and procedures that allows to configure and start/stop an L1 instance.
///
/// Note: This interface is considered a base as it lacks the operation controller that starts/stops the adaptor.
/// Note: This interface is not intended to be used on its self, it only serves as a common point for the other MAC-FAPI
/// P5 sector adaptor interfaces that contain an operation controller.
class mac_fapi_p5_sector_base_adaptor
{
public:
  virtual ~mac_fapi_p5_sector_base_adaptor() = default;

  /// Returns the FAPI configuration P5 responses notifier of this adaptor.
  virtual fapi::p5_responses_notifier& get_p5_responses_notifier() = 0;

  /// Returns the FAPI error indication notifier of this adaptor.
  virtual fapi::error_indication_notifier& get_error_indication_notifier() = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
