// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "ocudu/xnap/xnap_types.h"
#include "fmt/format.h"

namespace ocudu::ocucp {

class xnap_ue_log_prefix
{
public:
  xnap_ue_log_prefix(cu_cp_ue_index_t   ue_index,
                     local_xnap_ue_id_t local_xnap_ue_id = local_xnap_ue_id_t::invalid,
                     peer_xnap_ue_id_t  peer_xnap_ue_id  = peer_xnap_ue_id_t::invalid)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(
        std::back_inserter(buffer),
        "ue={}{}{}: ",
        ue_index,
        local_xnap_ue_id != local_xnap_ue_id_t::invalid ? fmt::format(" local_xnap_ue={}", local_xnap_ue_id) : "",
        peer_xnap_ue_id != peer_xnap_ue_id_t::invalid ? fmt::format(" peer_xnap_ue={}", peer_xnap_ue_id) : "");
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const xnap_ue_log_prefix& o)
{
  return o.to_c_str();
}

using xnap_ue_logger = prefixed_logger<xnap_ue_log_prefix>;

} // namespace ocudu::ocucp
