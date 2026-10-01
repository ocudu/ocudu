// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_adaptor.h"
#include <memory>

namespace ocudu {

namespace fapi {
class p7_last_request_notifier;
class p7_requests_gateway;
struct cell_configuration;
} // namespace fapi

class ru_controller;

namespace fapi_adaptor {

/// MAC-FAPI P7 sector adaptor abstract factory.
class mac_fapi_p7_sector_adaptor_factory
{
public:
  virtual ~mac_fapi_p7_sector_adaptor_factory() = default;

  /// Creates a MAC-FAPI P7 sector adaptor.
  virtual std::unique_ptr<mac_fapi_p7_sector_adaptor> create(const fapi::cell_configuration& fapi_cfg,
                                                             fapi::p7_requests_gateway&      p7_gateway,
                                                             fapi::p7_last_request_notifier& p7_last_req_notifier,
                                                             ru_controller&                  ru_ctrl) = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
