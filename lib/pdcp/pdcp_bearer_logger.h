// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rb_id.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace ocudu {

class pdcp_bearer_log_prefix
{
public:
  pdcp_bearer_log_prefix(uint32_t ue_index, rb_id_t rb_id, const char* dir)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "ue={} {} {}: ", ue_index, rb_id, dir);
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const pdcp_bearer_log_prefix& o)
{
  return o.to_c_str();
}

using pdcp_bearer_logger = prefixed_logger<pdcp_bearer_log_prefix>;

} // namespace ocudu
