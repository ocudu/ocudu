// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/positioning/positioning_ids.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu {
namespace ocucp {

class nrppa_meas_log_prefix
{
public:
  nrppa_meas_log_prefix(ran_meas_id_t ran_meas_id, lmf_meas_id_t lmf_meas_id)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
                   "ran_meas_id={} lmf_meas_id={}: ",
                   fmt::underlying(ran_meas_id),
                   fmt::underlying(lmf_meas_id));
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const nrppa_meas_log_prefix& o)
{
  return o.to_c_str();
}

using nrppa_meas_logger = prefixed_logger<nrppa_meas_log_prefix>;

} // namespace ocucp
} // namespace ocudu
