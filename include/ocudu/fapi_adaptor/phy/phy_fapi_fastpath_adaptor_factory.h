// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor_config.h"

namespace ocudu {
namespace fapi_adaptor {

/// Factory to create PHY-FAPI fastpath adaptors.
class phy_fapi_fastpath_adaptor_factory
{
public:
  virtual ~phy_fapi_fastpath_adaptor_factory() = default;

  /// Creates a PHY-FAPI fastpath adaptor using the given configuration and dependencies.
  virtual std::unique_ptr<phy_fapi_fastpath_adaptor> create(const phy_fapi_fastpath_adaptor_config& config,
                                                            phy_fapi_fastpath_adaptor_dependencies  dependencies) = 0;
};

/// Creates a PHY-FAPI fastpath adaptor factory.
std::unique_ptr<phy_fapi_fastpath_adaptor_factory> create_phy_fapi_fastpath_adaptor_factory();

} // namespace fapi_adaptor
} // namespace ocudu
