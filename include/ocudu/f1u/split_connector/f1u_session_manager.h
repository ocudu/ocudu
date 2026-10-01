// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_gateway.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/ran/s_nssai.h"
#include <map>

namespace ocudu {

using f1u_session_list        = std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>;
using f1u_five_qi_session_map = std::map<five_qi_t, f1u_session_list>;
using f1u_s_nssai_session_map = std::map<s_nssai_t, f1u_five_qi_session_map>;

struct f1u_session_maps {
  f1u_session_list        default_gw_sessions;
  f1u_s_nssai_session_map session_maps;
};

class f1u_session_manager
{
public:
  virtual ~f1u_session_manager()                                                           = default;
  virtual gtpu_tnl_pdu_session& get_next_f1u_gateway(s_nssai_t s_nssai, five_qi_t five_qi) = 0;
};

} // namespace ocudu
