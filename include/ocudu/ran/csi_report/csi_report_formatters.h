// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/csi_report/csi_report_size.h"
#include "ocudu/ran/uci/uci_formatters.h"
#include "fmt/std.h"

/// Custom formatter for \c ocudu::csi_report_configuration.
template <>
struct fmt::formatter<ocudu::csi_report_configuration> {
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
  auto format(const ocudu::csi_report_configuration& config, FormatContext& ctx) const
  {
    helper.format_always(ctx, "nof_csi_rs_resources={}", config.nof_csi_rs_resources);
    helper.format_always(ctx, "nof_reported_rs={}", config.nof_reported_rs);
    helper.format_always(ctx, "pmi_codebook={}", to_string(config.pmi_codebook));
    helper.format_always(ctx, "ri_restriction={}", config.ri_restriction);

    helper.format_always(ctx, "quantities={}", to_string(config.quantities));

    return ctx.out();
  }
};

/// Custom formatter for \c ocudu::precoding_matrix_indicator.
template <>
struct fmt::formatter<ocudu::precoding_matrix_indicator> {
  /// Helper used to parse formatting options and format the PMI fields.
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const ocudu::precoding_matrix_indicator& pmi, FormatContext& ctx) const
  {
    if (const auto* two_ports_pmi = std::get_if<ocudu::pmi_two_antenna_port>(&pmi)) {
      helper.format_always(ctx, "pmi={}", two_ports_pmi->pmi);
    } else if (const auto* four_ports_pmi = std::get_if<ocudu::pmi_typeI_single_panel>(&pmi)) {
      helper.format_always(ctx, "i_1_1={}", four_ports_pmi->i_1_1);
      helper.format_always(ctx, "i_1_2={}", four_ports_pmi->i_1_2);
      helper.format_always(ctx, "i_1_3={}", four_ports_pmi->i_1_3);
      helper.format_always(ctx, "i_2={}", four_ports_pmi->i_2);
    }

    return ctx.out();
  }
};

/// Custom formatter for \c ocudu::csi_report_data.
template <>
struct fmt::formatter<ocudu::csi_report_data> {
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
  auto format(const ocudu::csi_report_data& data, FormatContext& ctx) const
  {
    if (!data.cri.empty()) {
      helper.format_always(ctx, "cri=[{}]", data.cri);
    }

    if (!data.rsrp_dBm.empty()) {
      helper.format_always(ctx, "rsrp=[{}]dBm", data.rsrp_dBm);
    }

    if (data.ri.has_value()) {
      helper.format_always(ctx, "ri={}", data.ri.value());
    }

    if (data.li.has_value()) {
      helper.format_always(ctx, "li={}", data.li.value());
    }

    if (data.pmi.has_value()) {
      helper.format_always(ctx, "{}", data.pmi.value());
    }

    if (data.first_tb_wideband_cqi.has_value()) {
      helper.format_always(ctx, "cqi1={}", data.first_tb_wideband_cqi.value());
    }

    if (data.second_tb_wideband_cqi.has_value()) {
      helper.format_always(ctx, "cqi2={}", data.second_tb_wideband_cqi.value());
    }

    return ctx.out();
  }
};

/// Custom formatter for \c ocudu::csi_report_pusch_size.
template <>
struct fmt::formatter<ocudu::csi_report_size> {
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
  auto format(const ocudu::csi_report_size& data, FormatContext& ctx) const
  {
    helper.format_always(ctx, "part1={}", data.part1_size.value());
    helper.format_always(ctx, "part2={{{}}}", data.part2_correspondence);
    helper.format_always(ctx, "part2_min_size={}", data.part2_min_size);
    helper.format_always(ctx, "part2_max_size={}", data.part2_max_size);
    return ctx.out();
  }
};
