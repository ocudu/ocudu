// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../proc_logger.h"
#include "../ue_context/f1ap_ue_context.h"

namespace ocudu {
namespace odu {

struct f1ap_log_prefix : public ocudu::f1ap_common_log_prefix {
  using ocudu::f1ap_common_log_prefix::f1ap_common_log_prefix;

  f1ap_log_prefix(const f1ap_ue_context& context_, const char* proc_name_) :
    ocudu::f1ap_common_log_prefix(context_.gnb_du_ue_f1ap_id, context_.gnb_cu_ue_f1ap_id, proc_name_),
    ue_index(context_.ue_index)
  {
  }

  du_ue_index_t ue_index = INVALID_DU_UE_INDEX;
};

} // namespace odu
} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::odu::f1ap_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::odu::f1ap_log_prefix& prefix, FormatContext& ctx) const
  {
    bool needs_sep = prefix.ue_index != ocudu::INVALID_DU_UE_INDEX;
    if (prefix.ue_index != ocudu::INVALID_DU_UE_INDEX) {
      format_to(ctx.out(), "ue={}", prefix.ue_index);
    }
    return format_to(
        ctx.out(), "{}{}", needs_sep ? " " : "", static_cast<const ocudu::f1ap_common_log_prefix&>(prefix));
  }
};

} // namespace fmt
