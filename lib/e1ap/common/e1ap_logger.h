// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu {

class e1ap_log_prefix
{
public:
  e1ap_log_prefix(cu_up_e1_index_t e1_index)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "e1={}: ", fmt::underlying(e1_index));
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const e1ap_log_prefix& o)
{
  return o.to_c_str();
}

using e1ap_logger = prefixed_logger<e1ap_log_prefix>;

} // namespace ocudu
