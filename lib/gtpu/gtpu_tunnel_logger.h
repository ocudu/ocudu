// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gtpu/gtpu_logical_interface.h"
#include "ocudu/ran/gtpu/gtpu_teid.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include "fmt/std.h"

namespace ocudu {

class gtpu_tunnel_log_prefix
{
public:
  gtpu_tunnel_log_prefix(gtpu_logical_interface li, std::optional<uint32_t> ue_index, gtpu_teid_t teid, const char* dir)
  {
    fmt::memory_buffer buffer;
    if (ue_index.has_value()) {
      fmt::format_to(std::back_inserter(buffer), "li={} ue={} {} teid={}: ", li, *ue_index, dir, teid);
    } else {
      fmt::format_to(std::back_inserter(buffer), "li={} {} teid={}: ", li, dir, teid);
    }
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const gtpu_tunnel_log_prefix& o)
{
  return o.to_c_str();
}

using gtpu_tunnel_logger = prefixed_logger<gtpu_tunnel_log_prefix>;

} // namespace ocudu
