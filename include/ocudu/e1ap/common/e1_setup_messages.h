// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/ran/crit_diagnostics.h"
#include "ocudu/ran/cu_types.h"

namespace ocudu {

enum class cu_up_cn_support_t { c_epc = 0, c_5gc, both };

/// \brief CU-UP initiated E1 setup request.
struct cu_up_e1_setup_request {
  uint64_t                            gnb_cu_up_id;
  std::optional<std::string>          gnb_cu_up_name;
  cu_up_cn_support_t                  cn_support;
  std::vector<supported_plmns_item_t> supported_plmns;
  std::optional<uint16_t>             gnb_cu_up_capacity;

  unsigned max_setup_retries = 5;
};

/// \brief Response to CU-UP initiated E1 setup request.
struct cu_up_e1_setup_response {
  bool success = false;

  // e1 setup response
  std::optional<std::string> gnb_cu_cp_name;

  // e1 setup failure
  std::optional<e1ap_cause_t>       cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
  /// Packed E1AP PDUs captured during the setup exchange.
  byte_buffer packed_e1_setup_request;
  byte_buffer packed_e1_setup_response;
};

} // namespace ocudu
