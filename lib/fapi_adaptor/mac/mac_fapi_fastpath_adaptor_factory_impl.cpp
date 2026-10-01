// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_fastpath_adaptor_factory_impl.h"
#include "mac_fapi_fastpath_adaptor_impl.h"
#include "mac_fapi_sector_fastpath_adaptor_impl.h"
#include "p5/mac_fapi_p5_sector_fastpath_adaptor_impl.h"
#include "p7/mac_fapi_p7_sector_fastpath_adaptor_factory_impl.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_fastpath_adaptor_config.h"

using namespace ocudu;
using namespace fapi_adaptor;

std::unique_ptr<mac_fapi_fastpath_adaptor>
mac_fapi_fastpath_adaptor_factory_impl::create(const mac_fapi_fastpath_adaptor_config& config,
                                               mac_fapi_fastpath_adaptor_dependencies  dependencies)
{
  std::vector<std::unique_ptr<mac_fapi_sector_fastpath_adaptor>> sectors;
  for (unsigned i = 0, e = config.sectors.size(); i != e; ++i) {
    const auto& p5_sector_cfg          = config.sectors[i].p5_config;
    auto&       p5_sector_dependencies = dependencies.sectors[i].p5_dependencies;

    auto p5_adaptor =
        std::make_unique<mac_fapi_p5_sector_fastpath_adaptor_impl>(p5_sector_cfg, std::move(p5_sector_dependencies));

    const auto& p7_sector_cfg          = config.sectors[i].p7_config;
    auto&       p7_sector_dependencies = dependencies.sectors[i].p7_dependencies;

    // Create P7 fastpath adaptor.
    auto p7_adaptor = create_mac_fapi_p7_sector_fastpath_adaptor(
        p7_sector_cfg,
        mac_fapi_p7_sector_fastpath_adaptor_impl_dependencies{.base_dependencies = std::move(p7_sector_dependencies),
                                                              .slot_handler = p5_adaptor->get_mac_cell_slot_handler()});

    // Create sector adaptor.
    auto sector_adaptor =
        std::make_unique<mac_fapi_sector_fastpath_adaptor_impl>(std::move(p5_adaptor), std::move(p7_adaptor));

    sectors.push_back(std::move(sector_adaptor));
  }

  return std::make_unique<mac_fapi_fastpath_adaptor_impl>(std::move(sectors));
}

std::unique_ptr<mac_fapi_fastpath_adaptor_factory> ocudu::fapi_adaptor::create_mac_fapi_fastpath_adaptor_factory()
{
  return std::make_unique<mac_fapi_fastpath_adaptor_factory_impl>();
}
