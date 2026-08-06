// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/rrm/ue_capability_summary.h"
#include "ocudu/support/format/delimited_formatter.h"
#include <fmt/format.h>

namespace fmt {

/// \brief Custom formatter for \c ocudu::ue_capability_summary::supported_band.
template <>
struct formatter<ocudu::ue_capability_summary::supported_band> {
  /// Helper used to parse formatting options and format fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::ue_capability_summary::supported_band& params, FormatContext& ctx) const
  {
    helper.format_always(ctx, "pusch_qam256_supported={}", params.pusch_qam256_supported);
    helper.format_always(ctx, "pusch_tx_coherence={}", ocudu::to_string(params.pusch_tx_coherence));
    helper.format_always(ctx, "pusch_max_rank={}", params.pusch_max_rank);
    helper.format_always(ctx, "nof_srs_tx_ports={}", params.nof_srs_tx_ports);
    helper.format_always(ctx, "max_dl_harq_process_num={}", params.max_dl_harq_process_num);
    helper.format_always(ctx, "max_ul_harq_process_num={}", params.max_ul_harq_process_num);
    helper.format_always(ctx, "ul_pre_compensation_supported={}", params.ul_pre_compensation_supported);
    helper.format_always(ctx, "ul_ta_reporting_supported={}", params.ul_ta_reporting_supported);
    helper.format_always(ctx, "ue_specific_k_offset_supported={}", params.ue_specific_k_offset_supported);
    helper.format_always(ctx, "max_pdsch_tdra_rep_number={}", params.max_pdsch_tdra_rep_number);
    helper.format_always(ctx, "pusch_rep_type_a_avail_slot_supported={}", params.pusch_rep_type_a_avail_slot_supported);
    helper.format_always(ctx, "pucch_repeat_f0_2_r17_supported={}", params.pucch_repeat_f0_2_r17_supported);
    if (params.type2_codebook.has_value()) {
      helper.format_always(ctx,
                           "type2_codebook={{max_nof_beams={} subband_amplitude_supported={} "
                           "max_nof_tx_ports_per_resource={}}}",
                           params.type2_codebook->max_nof_beams,
                           params.type2_codebook->subband_amplitude_supported,
                           params.type2_codebook->max_nof_tx_ports_per_resource);
    } else {
      helper.format_always(ctx, "type2_codebook=na");
    }
    return ctx.out();
  }
};

/// \brief Custom formatter for \c ocudu::ue_capability_summary.
template <>
struct formatter<ocudu::ue_capability_summary> {
  /// Helper used to parse formatting options and format fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::ue_capability_summary& params, FormatContext& ctx) const
  {
    helper.format_always(ctx, "pdsch_qam256_supported={}", params.pdsch_qam256_supported);
    helper.format_always(ctx, "pdsch_qam64lowse_supported={}", params.pdsch_qam64lowse_supported);
    helper.format_always(ctx, "pusch_qam64lowse_supported={}", params.pusch_qam64lowse_supported);
    helper.format_always(ctx, "pusch_rep_type_a_supported={}", params.pusch_rep_type_a_supported);
    for (const auto& band : params.bands) {
      helper.format_always(ctx, "n{}={{{}}}", fmt::underlying(band.first), band.second);
    }
    helper.format_always(ctx, "long_drx_cycle_supported={}", params.long_drx_cycle_supported);
    helper.format_always(ctx, "short_drx_cycle_supported={}", params.short_drx_cycle_supported);
    helper.format_always(
        ctx, "pdsch_interleaving_vrb_to_prb_supported={}", params.pdsch_interleaving_vrb_to_prb_supported);
    helper.format_always(ctx, "ntn_supported={}", params.ntn_supported);
    helper.format_always(ctx, "disabled_dl_harq_feedback_supported={}", params.disabled_dl_harq_feedback_supported);
    helper.format_always(ctx, "ul_harq_mode_b_supported={}", params.ul_harq_mode_b_supported);
    helper.format_always(ctx, "supported_gap_patterns={}", params.supported_meas_gaps.bits());
    helper.format_always(ctx, "pucch_repeat_f1_3_4_supported={}", params.pucch_repeat_f1_3_4_supported);
    helper.format_always(
        ctx, "slot_based_dyn_pucch_rep_r17_supported={}", params.slot_based_dyn_pucch_rep_r17_supported);
    return ctx.out();
  }
};

} // namespace fmt
