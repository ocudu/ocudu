// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "p5_requests_handler.h"
#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_adaptor.h"

namespace ocudu {
namespace fapi_adaptor {

/// PHY-FAPI P5 sector fastpath adaptor implementation.
class phy_fapi_p5_sector_fastpath_adaptor_impl : public phy_fapi_p5_sector_adaptor
{
public:
  phy_fapi_p5_sector_fastpath_adaptor_impl(const phy_fapi_p5_sector_fastpath_adaptor_config&       config,
                                           const phy_fapi_p5_sector_fastpath_adaptor_dependencies& dependencies);

  // See interface for documentation.
  fapi::p5_requests_gateway& get_p5_requests_gateway() override;

  // See interface for documentation.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& config_notifier) override;

  // See interface for documentation.
  void set_error_indication_notifier(fapi::error_indication_notifier& err_notifier) override;

private:
  p5_requests_handler p5_handler;
};

} // namespace fapi_adaptor
} // namespace ocudu
