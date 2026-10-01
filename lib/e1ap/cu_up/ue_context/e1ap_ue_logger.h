// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_types.h"
#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/cu_up_types.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace ocudu {
namespace ocuup {

class e1ap_ue_log_prefix
{
public:
  e1ap_ue_log_prefix(cu_up_ue_index_t       ue_index,
                     gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid,
                     gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
                   "ue={}{}{}: ",
                   ue_index,
                   cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid
                       ? fmt::format(" cu_up_ue_e1ap_id={}", fmt::underlying(cu_up_ue_e1ap_id))
                       : "",
                   cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid
                       ? fmt::format(" cu_cp_ue_e1ap_id={}", fmt::underlying(cu_cp_ue_e1ap_id))
                       : "");
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const e1ap_ue_log_prefix& o)
{
  return o.to_c_str();
}

using e1ap_ue_logger = prefixed_logger<e1ap_ue_log_prefix>;

} // namespace ocuup
} // namespace ocudu
