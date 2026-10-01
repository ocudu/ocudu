// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor_factory.h"
#include <memory>

namespace ocudu {

namespace fapi_adaptor {

/// Implementation of the PHY-FAPI adaptor factory.
class phy_fapi_fastpath_adaptor_factory_impl : public phy_fapi_fastpath_adaptor_factory
{
public:
  // See interface for documentation.
  std::unique_ptr<phy_fapi_fastpath_adaptor> create(const phy_fapi_fastpath_adaptor_config& config,
                                                    phy_fapi_fastpath_adaptor_dependencies  dependencies) override;
};

} // namespace fapi_adaptor
} // namespace ocudu
