// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_fapi_sector_fastpath_adaptor_impl.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace fapi_adaptor;

phy_fapi_sector_fastpath_adaptor_impl::phy_fapi_sector_fastpath_adaptor_impl(
    std::unique_ptr<phy_fapi_p5_sector_adaptor>          p5_adaptor_,
    std::unique_ptr<phy_fapi_p7_sector_fastpath_adaptor> p7_adaptor_) :
  p5_adaptor(std::move(p5_adaptor_)), p7_adaptor(std::move(p7_adaptor_))
{
  ocudu_assert(p5_adaptor, "Invalid PHY-FAPI P5 sector adaptor");
  ocudu_assert(p7_adaptor, "Invalid PHY-FAPI P7 sector adaptor");
}

phy_fapi_p5_sector_adaptor& phy_fapi_sector_fastpath_adaptor_impl::get_p5_sector_adaptor()
{
  return *p5_adaptor;
}

phy_fapi_p7_sector_fastpath_adaptor& phy_fapi_sector_fastpath_adaptor_impl::get_p7_sector_adaptor()
{
  return *p7_adaptor;
}
