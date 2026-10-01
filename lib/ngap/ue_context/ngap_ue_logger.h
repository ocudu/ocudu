// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/ngap_types.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu::ocucp {

class ngap_ue_log_prefix
{
public:
  ngap_ue_log_prefix(cu_cp_ue_index_t ue_index,
                     ran_ue_id_t      ran_ue_id = ran_ue_id_t::invalid,
                     amf_ue_id_t      amf_ue_id = amf_ue_id_t::invalid)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
                   "ue={}{}{}: ",
                   ue_index,
                   ran_ue_id != ran_ue_id_t::invalid ? fmt::format(" ran_ue={}", fmt::underlying(ran_ue_id)) : "",
                   amf_ue_id != amf_ue_id_t::invalid ? fmt::format(" amf_ue={}", amf_ue_id) : "");
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const ngap_ue_log_prefix& o)
{
  return o.to_c_str();
}

using ngap_ue_logger = prefixed_logger<ngap_ue_log_prefix>;

} // namespace ocudu::ocucp
