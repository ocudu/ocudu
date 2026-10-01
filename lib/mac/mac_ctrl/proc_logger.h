// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

struct mac_log_prefix {
  mac_log_prefix(du_ue_index_t ue_idx, rnti_t rnti_, const char* proc_name_) :
    ue_index(ue_idx), rnti(rnti_), proc_name(proc_name_)
  {
  }

  du_ue_index_t ue_index  = INVALID_DU_UE_INDEX;
  rnti_t        rnti      = rnti_t::INVALID_RNTI;
  const char*   proc_name = nullptr;
};

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::mac_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::mac_log_prefix& prefix, FormatContext& ctx) const
  {
    bool first_id = true;
    auto get_sep  = [&first_id]() { return std::exchange(first_id, false) ? "" : " "; };
    if (prefix.ue_index != ocudu::INVALID_DU_UE_INDEX) {
      format_to(ctx.out(), "{}ue={}", get_sep(), prefix.ue_index);
    }
    if (prefix.rnti != ocudu::rnti_t::INVALID_RNTI) {
      format_to(ctx.out(), "{}crnti={}", get_sep(), prefix.rnti);
    }
    format_to(ctx.out(), "{}proc=\"{}\"", get_sep(), prefix.proc_name);
    return ctx.out();
  }
};

} // namespace fmt
