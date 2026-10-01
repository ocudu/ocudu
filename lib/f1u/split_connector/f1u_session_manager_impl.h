// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/split_connector/f1u_session_manager.h"

namespace ocudu {

class f1u_session_manager_impl : public f1u_session_manager
{
public:
  ~f1u_session_manager_impl() override = default;
  explicit f1u_session_manager_impl(const f1u_session_maps& f1u_sessions_);

  gtpu_tnl_pdu_session& get_next_f1u_gateway(s_nssai_t s_nssai, five_qi_t five_qi) override;

private:
  ocudulog::basic_logger&                            logger;
  const f1u_session_maps&                            f1u_sessions;
  uint32_t                                           next_gw = 0;
  std::map<s_nssai_t, std::map<five_qi_t, uint32_t>> next_gw_map;
};

} // namespace ocudu
