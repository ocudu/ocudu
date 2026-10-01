// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/format/prefixed_logger.h"
#include "fmt/format.h"

namespace ocudu {

class sdap_session_trx_log_prefix
{
public:
  sdap_session_trx_log_prefix(uint32_t                     ue_index,
                              pdu_session_id_t             psi,
                              std::optional<qos_flow_id_t> qfi,
                              drb_id_t                     drb_id,
                              const char*                  dir)
  {
    fmt::memory_buffer buffer;
    if (qfi.has_value()) {
      fmt::format_to(std::back_inserter(buffer), "ue={} {} {} {} {}: ", ue_index, psi, qfi.value(), drb_id, dir);
    } else {
      fmt::format_to(std::back_inserter(buffer), "ue={} {} {} {}: ", ue_index, psi, drb_id, dir);
    }
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const sdap_session_trx_log_prefix& o)
{
  return o.to_c_str();
}

using sdap_session_trx_logger = prefixed_logger<sdap_session_trx_log_prefix>;

class sdap_session_log_prefix
{
public:
  sdap_session_log_prefix(uint32_t ue_index, pdu_session_id_t psi)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "ue={} {}: ", ue_index, psi);
    prefix = ocudu::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

inline const char* format_as(const sdap_session_log_prefix& o)
{
  return o.to_c_str();
}

using sdap_session_logger = prefixed_logger<sdap_session_log_prefix>;

} // namespace ocudu
