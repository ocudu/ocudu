// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "configuration_procedure.h"
#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_adaptor.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief PHY-FAPI P5 sector operation request adaptor.
///
/// This P5 sector adaptor allows to configure a cell operation request notifier, that will be notified in every
/// START.request and STOP.request FAPI messages.
class phy_fapi_p5_sector_operation_request_adaptor : public phy_fapi_p5_sector_adaptor
{
  fapi::configuration_procedure gateway;

public:
  explicit phy_fapi_p5_sector_operation_request_adaptor(ocudulog::basic_logger& logger) : gateway(logger) {}

  // See interface for documentation.
  fapi::p5_requests_gateway& get_p5_requests_gateway() override;

  // See interface for documentation.
  void set_error_indication_notifier(fapi::error_indication_notifier& err_notifier) override;

  // See interface for documentation.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& config_notifier) override;

  // Sets the FAPI P5 operational change request notifier to the given one.
  void set_p5_operational_change_request_notifier(fapi::p5_operational_change_request_notifier& p5_notifier);
};

} // namespace fapi_adaptor
} // namespace ocudu
