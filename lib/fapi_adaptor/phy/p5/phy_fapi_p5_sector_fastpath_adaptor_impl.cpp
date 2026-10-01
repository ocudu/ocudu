// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_fapi_p5_sector_fastpath_adaptor_impl.h"

using namespace ocudu;
using namespace fapi_adaptor;

phy_fapi_p5_sector_fastpath_adaptor_impl::phy_fapi_p5_sector_fastpath_adaptor_impl(
    const phy_fapi_p5_sector_fastpath_adaptor_config&       config,
    const phy_fapi_p5_sector_fastpath_adaptor_dependencies& dependencies) :
  p5_handler(config, dependencies)
{
}

fapi::p5_requests_gateway& phy_fapi_p5_sector_fastpath_adaptor_impl::get_p5_requests_gateway()
{
  return p5_handler;
}

void phy_fapi_p5_sector_fastpath_adaptor_impl::set_p5_responses_notifier(fapi::p5_responses_notifier& config_notifier)
{
  p5_handler.set_p5_responses_notifier(config_notifier);
}

void phy_fapi_p5_sector_fastpath_adaptor_impl::set_error_indication_notifier(
    fapi::error_indication_notifier& err_notifier)
{
  p5_handler.set_error_indication_notifier(err_notifier);
}
