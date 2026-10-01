// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_types.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/support/format/fmt_basic_parser.h"

namespace ocudu::ocucp {

/// IDs that identify an E1AP UE context.
struct e1ap_ue_ids {
  /// UE identifier that is internal to the CU-CP.
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  /// CU-CP-UE-E1AP-ID.
  const gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  /// CU-UP-UE-E1AP-ID.
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;
};

} // namespace ocudu::ocucp

namespace fmt {

template <>
struct formatter<ocudu::ocucp::e1ap_ue_ids> : public basic_parser {
  template <typename FormatContext>
  auto format(const ocudu::ocucp::e1ap_ue_ids& p, FormatContext& ctx) const
  {
    using namespace ocudu;
    using namespace ocudu::ocucp;
    const char* sep = "";

    if (p.ue_index != cu_cp_ue_index_t::invalid) {
      fmt::format_to(ctx.out(), "ue={}", p.ue_index);
      sep = " ";
    }
    if (p.cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid) {
      fmt::format_to(ctx.out(), "{}cu_cp_ue={}", sep, fmt::underlying(p.cu_cp_ue_e1ap_id));
      sep = " ";
    }
    if (p.cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid) {
      fmt::format_to(ctx.out(), "{}cu_up_ue={}", sep, fmt::underlying(p.cu_up_ue_e1ap_id));
    }
    return ctx.out();
  }
};

} // namespace fmt
