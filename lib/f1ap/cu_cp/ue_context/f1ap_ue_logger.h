// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../proc_logger.h"
#include "f1ap_ue_ids.h"
#include "ocudu/support/format/prefixed_logger.h"

namespace ocudu::ocucp {

/// Log prefix for UEs in F1AP-CU.
struct f1ap_ue_log_prefix : public ocudu::f1ap_common_log_prefix {
  using ocudu::f1ap_common_log_prefix::f1ap_common_log_prefix;

  f1ap_ue_log_prefix(cu_cp_ue_index_t                   ue_index_,
                     gnb_cu_ue_f1ap_id_t                cu_ue_id_,
                     std::optional<gnb_du_ue_f1ap_id_t> du_ue_id_) :
    ocudu::f1ap_common_log_prefix(du_ue_id_.value_or(gnb_du_ue_f1ap_id_t::invalid), cu_ue_id_), ue_index(ue_index_)
  {
  }
  f1ap_ue_log_prefix(const f1ap_ue_ids& context_, const char* proc_name_ = nullptr) :
    ocudu::f1ap_common_log_prefix(context_.du_ue_f1ap_id.value_or(gnb_du_ue_f1ap_id_t::invalid),
                                  context_.cu_ue_f1ap_id,
                                  proc_name_),
    ue_index(context_.ue_index)
  {
  }

  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
};

/// Logger that automatically prefixes log messages with the UE IDs.
using f1ap_ue_logger = prefixed_logger<f1ap_ue_log_prefix>;

} // namespace ocudu::ocucp

namespace fmt {

template <>
struct formatter<ocudu::ocucp::f1ap_ue_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::ocucp::f1ap_ue_log_prefix& prefix, FormatContext& ctx) const
  {
    using namespace ocudu;
    bool needs_sep = prefix.ue_index != cu_cp_ue_index_t::invalid;
    if (prefix.ue_index != cu_cp_ue_index_t::invalid) {
      format_to(ctx.out(), "ue={}", prefix.ue_index);
    }
    return format_to(ctx.out(), "{}{}", needs_sep ? " " : "", static_cast<const f1ap_common_log_prefix&>(prefix));
  }
};

} // namespace fmt
