// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "xnu_session_manager.h"

namespace ocudu::ocuup {

class xnu_session_manager_impl : public xnu_session_manager
{
public:
  ~xnu_session_manager_impl() override = default;
  explicit xnu_session_manager_impl(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& xnu_gws_);

  gtpu_tnl_pdu_session& get_next_xnu_gateway() override;

private:
  const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& xnu_gws;
  uint32_t                                                  next_gw = 0;
};

} // namespace ocudu::ocuup
