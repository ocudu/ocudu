// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/positioning/positioning_ids.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu::ocucp {

class nrppa_ue_log_prefix
{
public:
  nrppa_ue_log_prefix(cu_cp_ue_index_t ue_index, ran_ue_meas_id_t ran_ue_meas_id, lmf_ue_meas_id_t lmf_ue_meas_id)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
                   "ue={} ran_ue={} lmf_ue={}: ",
                   ue_index,
                   fmt::underlying(ran_ue_meas_id),
                   fmt::underlying(lmf_ue_meas_id));
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const nrppa_ue_log_prefix& o)
{
  return o.to_c_str();
}

using nrppa_ue_logger = prefixed_logger<nrppa_ue_log_prefix>;

} // namespace ocudu::ocucp
