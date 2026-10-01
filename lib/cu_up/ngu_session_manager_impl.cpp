// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngu_session_manager_impl.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ocuup;

ngu_session_manager_impl::ngu_session_manager_impl(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& ngu_gws_) :
  ngu_gws(ngu_gws_)
{
  ocudu_assert(not ngu_gws.empty(), "NG-U gateways cannot be empty");
}

gtpu_tnl_pdu_session& ngu_session_manager_impl::get_next_ngu_gateway()
{
  uint32_t index = next_gw % ngu_gws.size();
  next_gw++;
  return *ngu_gws[index];
}
