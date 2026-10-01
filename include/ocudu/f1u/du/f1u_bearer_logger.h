// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace ocudu {
namespace odu {

class f1u_bearer_log_prefix
{
public:
  f1u_bearer_log_prefix(uint32_t ue_index, drb_id_t drb_id, const up_transport_layer_info& dl_tnl_info)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
                   "ue={} {} addr={} dl-teid={}: ",
                   ue_index,
                   drb_id,
                   dl_tnl_info.tp_address,
                   dl_tnl_info.gtp_teid);
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const f1u_bearer_log_prefix& o)
{
  return o.to_c_str();
}

using f1u_bearer_logger = prefixed_logger<f1u_bearer_log_prefix>;

} // namespace odu
} // namespace ocudu
