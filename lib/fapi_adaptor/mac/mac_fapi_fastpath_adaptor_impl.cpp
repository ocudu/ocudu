// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_fapi_fastpath_adaptor_impl.h"
#include "ocudu/fapi_adaptor/mac/operation_controller.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace fapi_adaptor;

mac_fapi_fastpath_adaptor_impl::mac_fapi_fastpath_adaptor_impl(
    std::vector<std::unique_ptr<mac_fapi_sector_fastpath_adaptor>> sector_adaptors_) :
  sector_adaptors(std::move(sector_adaptors_))
{
  ocudu_assert(!sector_adaptors.empty(), "Cannot create a MAC FAPI adaptor with zero cells");
}

mac_fapi_sector_fastpath_adaptor& mac_fapi_fastpath_adaptor_impl::get_sector_adaptor(unsigned cell_id)
{
  ocudu_assert(cell_id < sector_adaptors.size(),
               "Invalid cell identifier '{}'. Valid cell id range '[0-{})'",
               cell_id,
               sector_adaptors.size());
  return *sector_adaptors[cell_id];
}

void mac_fapi_fastpath_adaptor_impl::stop()
{
  for (auto& sector : sector_adaptors) {
    sector->get_operation_controller().stop();
  }
}
