// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"
#include "fmt/format.h"

namespace ocudu {
namespace odu {

/// UE F1 context that is referenced by both the F1 UE object and its bearers.
struct f1ap_ue_context {
  /// Different stages of a UE RRC configuration.
  enum class ue_rrc_state { no_config, config_pending, config_applied };

  const du_ue_index_t       ue_index;
  const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id;
  gnb_cu_ue_f1ap_id_t       gnb_cu_ue_f1ap_id  = gnb_cu_ue_f1ap_id_t::invalid;
  rnti_t                    rnti               = rnti_t::INVALID_RNTI;
  ue_rrc_state              rrc_state          = ue_rrc_state::no_config;
  bool                      marked_for_release = false;
  du_cell_index_t           sp_cell_index      = INVALID_DU_CELL_INDEX;

  f1ap_ue_context(du_ue_index_t ue_index_, gnb_du_ue_f1ap_id_t du_f1ap_ue_id_) :
    ue_index(ue_index_), gnb_du_ue_f1ap_id(du_f1ap_ue_id_)
  {
  }
};

} // namespace odu
} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::odu::f1ap_ue_context> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::odu::f1ap_ue_context& ue, FormatContext& ctx) const
  {
    if (ue.gnb_cu_ue_f1ap_id == ocudu::gnb_cu_ue_f1ap_id_t::invalid) {
      return format_to(
          ctx.out(), "ue={} c-rnti={} du_ue={}", ue.ue_index, ue.rnti, fmt::underlying(ue.gnb_du_ue_f1ap_id));
    }
    return format_to(ctx.out(),
                     "ue={} c-rnti={} du_ue={} cu_ue={}",
                     ue.ue_index,
                     ue.rnti,
                     fmt::underlying(ue.gnb_du_ue_f1ap_id),
                     fmt::underlying(ue.gnb_cu_ue_f1ap_id));
  }
};

} // namespace fmt
