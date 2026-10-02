// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnu_session_manager_impl.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocuup;

xnu_session_manager_impl::xnu_session_manager_impl(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& xnu_gws_) :
  xnu_gws(xnu_gws_)
{
  ocudu_assert(not xnu_gws.empty(), "Xn-U gateways cannot be empty");
}

gtpu_tnl_pdu_session& xnu_session_manager_impl::get_next_xnu_gateway()
{
  uint32_t index = next_gw % xnu_gws.size();
  next_gw++;
  return *xnu_gws[index];
}
