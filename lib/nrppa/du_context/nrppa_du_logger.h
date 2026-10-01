// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu::ocucp {

class nrppa_du_log_prefix
{
public:
  nrppa_du_log_prefix(cu_cp_du_index_t du_index)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "du={}: ", du_index);
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const nrppa_du_log_prefix& o)
{
  return o.to_c_str();
}

using nrppa_du_logger = prefixed_logger<nrppa_du_log_prefix>;

} // namespace ocudu::ocucp
