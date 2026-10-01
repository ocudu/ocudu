// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_fapi_fastpath_adaptor_factory_impl.h"
#include "p5/phy_fapi_p5_sector_fastpath_adaptor_impl.h"
#include "p7/phy_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "phy_fapi_fastpath_adaptor_impl.h"
#include "phy_fapi_sector_fastpath_adaptor_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_fastpath_adaptor>
phy_fapi_fastpath_adaptor_factory_impl::create(const phy_fapi_fastpath_adaptor_config& config,
                                               phy_fapi_fastpath_adaptor_dependencies  dependencies)
{
  std::vector<std::unique_ptr<phy_fapi_sector_fastpath_adaptor>> sectors;
  for (unsigned i = 0, e = config.sectors.size(); i != e; ++i) {
    const auto& p5_sector_cfg          = config.sectors[i].p5_config;
    const auto& p5_sector_dependencies = dependencies.sectors[i].p5_dependencies;
    auto p5_adaptor = std::make_unique<phy_fapi_p5_sector_fastpath_adaptor_impl>(p5_sector_cfg, p5_sector_dependencies);

    const auto& p7_sector_cfg          = config.sectors[i].p7_config;
    auto&       p7_sector_dependencies = dependencies.sectors[i].p7_dependencies;

    sectors.push_back(std::make_unique<phy_fapi_sector_fastpath_adaptor_impl>(
        std::move(p5_adaptor),
        std::make_unique<phy_fapi_p7_sector_fastpath_adaptor_impl>(p7_sector_cfg, std::move(p7_sector_dependencies))));
  }

  return std::make_unique<phy_fapi_fastpath_adaptor_impl>(std::move(sectors));
}

std::unique_ptr<phy_fapi_fastpath_adaptor_factory> ocudu::fapi_adaptor::create_phy_fapi_fastpath_adaptor_factory()
{
  return std::make_unique<phy_fapi_fastpath_adaptor_factory_impl>();
}
