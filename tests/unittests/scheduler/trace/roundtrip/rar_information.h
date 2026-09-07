// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/scheduler/result/vrb_alloc.h"
#include <gtest/gtest.h>

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct roundtrip_traits<rar_ul_grant::four_step_info> {
  static constexpr auto members = std::make_tuple();
};

template <>
struct roundtrip_traits<rar_ul_grant::two_step_success_info> {
  static constexpr auto members = std::make_tuple(
      field("harq_feedback_timing_indicator", &rar_ul_grant::two_step_success_info::harq_feedback_timing_indicator),
      field("pucch_resource_indicator", &rar_ul_grant::two_step_success_info::pucch_resource_indicator));
};

template <>
struct roundtrip_traits<rar_ul_grant::two_step_fallback_info> {
  static constexpr auto members = std::make_tuple();
};

template <>
struct roundtrip_traits<rar_ul_grant> {
  static constexpr auto members =
      std::make_tuple(field("temp_crnti", &rar_ul_grant::temp_crnti),
                      field("rapid", &rar_ul_grant::rapid),
                      field("ta", &rar_ul_grant::ta),
                      field("time_resource_assignment", &rar_ul_grant::time_resource_assignment),
                      field("freq_hop_flag", &rar_ul_grant::freq_hop_flag),
                      field("freq_resource_assignment", &rar_ul_grant::freq_resource_assignment),
                      field("mcs", &rar_ul_grant::mcs),
                      field("tpc", &rar_ul_grant::tpc),
                      field("csi_req", &rar_ul_grant::csi_req),
                      field("type", &rar_ul_grant::type));
};

// fbs::RarPdsch carries rnti/rbs/symbols/one codeword -- harq_id etc. are UE-grant-only; backoff_indicator isn't
// serialized either. See sib_information.h for why computed_field is used here.
template <>
struct roundtrip_traits<rar_information> {
  static constexpr auto members = std::make_tuple(
      computed_field<rar_information, rnti_t>("rnti", [](rar_information& r) -> rnti_t& { return r.pdsch_cfg.rnti; }),
      computed_field<rar_information, const bwp_configuration*>(
          "bwp_cfg",
          [](rar_information& r) -> const bwp_configuration*& { return r.pdsch_cfg.bwp_cfg; }),
      computed_field<rar_information, vrb_alloc>("rbs",
                                                 [](rar_information& r) -> vrb_alloc& { return r.pdsch_cfg.rbs; }),
      computed_field<rar_information, ofdm_symbol_range>(
          "symbols",
          [](rar_information& r) -> ofdm_symbol_range& { return r.pdsch_cfg.symbols; }),
      computed_field<rar_information, uint8_t>(
          "rv_index",
          [](rar_information& r) -> uint8_t& { return r.pdsch_cfg.codewords[0].rv_index; }),
      computed_field<rar_information, sch_mcs_index>(
          "mcs_index",
          [](rar_information& r) -> sch_mcs_index& { return r.pdsch_cfg.codewords[0].mcs_index; }),
      computed_field<rar_information, units::bytes>(
          "tb_size_bytes",
          [](rar_information& r) -> units::bytes& { return r.pdsch_cfg.codewords[0].tb_size_bytes; }),
      field("grants", &rar_information::grants));

  static rar_information roundtrip(const rar_information& rar)
  {
    return roundtrip_via<fbs::RarPdsch>(
        rar, convert_rar_pdsch_to_fb, [](rar_information& out, const fbs::RarPdsch& fb) {
          const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
          convert_fb_to_rar_pdsch(out, fb, &cell_cfg.init_dl_bwp);
        });
  }
};

// Custom base with a codeword already pushed, since the composite auto-sweep would index codewords[0] before it exists.
template <>
struct test_values<rar_information> {
  static std::vector<rar_information> get()
  {
    rar_information rar{};
    rar.pdsch_cfg.rnti    = rnti_t::INVALID_RNTI;
    rar.pdsch_cfg.bwp_cfg = &test_helper::make_test_schedtrace_cell_cfg().init_dl_bwp;
    rar.pdsch_cfg.rbs     = vrb_alloc(vrb_interval{0, 0});
    rar.pdsch_cfg.symbols = ofdm_symbol_range{0, 1};
    pdsch_codeword cw{};
    cw.rv_index      = 0;
    cw.mcs_index     = sch_mcs_index{0};
    cw.tb_size_bytes = units::bytes{0};
    rar.pdsch_cfg.codewords.push_back(cw);
    return field_variations_from(rar);
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
