// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_types.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu {
namespace ocuup {

class cu_up_log_prefix
{
public:
  cu_up_log_prefix(cu_up_ue_index_t ue_index)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "ue={}: ", ue_index);
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const cu_up_log_prefix& o)
{
  return o.to_c_str();
}

using cu_up_ue_logger = prefixed_logger<cu_up_log_prefix>;

} // namespace ocuup
} // namespace ocudu
