// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "fmt/format.h"
#include <utility>

namespace ocudu {

struct f1ap_common_log_prefix {
  f1ap_common_log_prefix(gnb_du_ue_f1ap_id_t du_ue_id_, const char* proc_name_ = nullptr) :
    du_ue_id(du_ue_id_), proc_name(proc_name_)
  {
  }
  f1ap_common_log_prefix(gnb_du_ue_f1ap_id_t du_ue_id_,
                         gnb_cu_ue_f1ap_id_t cu_ue_id_,
                         const char*         proc_name_ = nullptr) :
    du_ue_id(du_ue_id_), cu_ue_id(cu_ue_id_), proc_name(proc_name_)
  {
  }
  f1ap_common_log_prefix(gnb_cu_ue_f1ap_id_t cu_ue_id_, const char* proc_name_ = nullptr) :
    cu_ue_id(cu_ue_id_), proc_name(proc_name_)
  {
  }

  gnb_du_ue_f1ap_id_t du_ue_id  = gnb_du_ue_f1ap_id_t::invalid;
  gnb_cu_ue_f1ap_id_t cu_ue_id  = gnb_cu_ue_f1ap_id_t::invalid;
  const char*         proc_name = nullptr;
};

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::f1ap_common_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::f1ap_common_log_prefix& prefix, FormatContext& ctx) const
  {
    bool first_id = true;
    auto get_sep  = [&first_id]() { return std::exchange(first_id, false) ? "" : " "; };
    if (prefix.du_ue_id != ocudu::gnb_du_ue_f1ap_id_t::invalid) {
      format_to(ctx.out(), "{}du_ue={}", get_sep(), fmt::underlying(prefix.du_ue_id));
    }
    if (prefix.cu_ue_id != ocudu::gnb_cu_ue_f1ap_id_t::invalid) {
      format_to(ctx.out(), "{}cu_ue={}", get_sep(), fmt::underlying(prefix.cu_ue_id));
    }
    if (prefix.proc_name != nullptr) {
      format_to(ctx.out(), "{}proc=\"{}\"", get_sep(), prefix.proc_name);
    }
    return ctx.out();
  }
};

} // namespace fmt
