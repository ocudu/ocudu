// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_fapi_p5_sector_operation_request_adaptor.h"

using namespace ocudu;
using namespace fapi_adaptor;

fapi::p5_requests_gateway& phy_fapi_p5_sector_operation_request_adaptor::get_p5_requests_gateway()
{
  return gateway;
}

void phy_fapi_p5_sector_operation_request_adaptor::set_p5_responses_notifier(
    fapi::p5_responses_notifier& config_notifier)
{
  gateway.set_p5_responses_notifier(config_notifier);
}

void phy_fapi_p5_sector_operation_request_adaptor::set_error_indication_notifier(
    fapi::error_indication_notifier& err_notifier)
{
  gateway.set_error_indication_notifier(err_notifier);
}

void phy_fapi_p5_sector_operation_request_adaptor::set_p5_operational_change_request_notifier(
    fapi::p5_operational_change_request_notifier& p5_notifier)
{
  gateway.set_cell_operation_request_notifier(p5_notifier);
}
