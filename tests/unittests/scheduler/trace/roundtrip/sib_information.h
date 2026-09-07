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
struct test_values<sib_information::si_indicator_type> {
  static std::vector<sib_information::si_indicator_type> get()
  {
    return {sib_information::sib1, sib_information::other_si};
  }
};

// fbs::SibPdsch only carries rbs/symbols/one codeword -- rnti, harq_id etc. are UE-grant-only and unset here.
template <>
struct roundtrip_traits<sib_information> {
  static constexpr auto members = std::make_tuple(
      field("si_indicator", &sib_information::si_indicator),
      computed_field<sib_information, const bwp_configuration*>(
          "bwp_cfg",
          [](sib_information& s) -> const bwp_configuration*& { return s.pdsch_cfg.bwp_cfg; }),
      computed_field<sib_information, vrb_alloc>("rbs",
                                                 [](sib_information& s) -> vrb_alloc& { return s.pdsch_cfg.rbs; }),
      computed_field<sib_information, ofdm_symbol_range>(
          "symbols",
          [](sib_information& s) -> ofdm_symbol_range& { return s.pdsch_cfg.symbols; }),
      computed_field<sib_information, uint8_t>(
          "rv_index",
          [](sib_information& s) -> uint8_t& { return s.pdsch_cfg.codewords[0].rv_index; }),
      computed_field<sib_information, sch_mcs_index>(
          "mcs_index",
          [](sib_information& s) -> sch_mcs_index& { return s.pdsch_cfg.codewords[0].mcs_index; }),
      computed_field<sib_information, units::bytes>("tb_size_bytes", [](sib_information& s) -> units::bytes& {
        return s.pdsch_cfg.codewords[0].tb_size_bytes;
      }));

  static sib_information roundtrip(const sib_information& sib)
  {
    return roundtrip_via<fbs::SibPdsch>(
        sib, convert_sib_pdsch_to_fb, [](sib_information& out, const fbs::SibPdsch& fb) {
          const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
          convert_fb_to_sib_pdsch(out, fb, &cell_cfg.init_dl_bwp);
        });
  }
};

// Custom base with a codeword already pushed, since the composite auto-sweep would index codewords[0] before it exists.
template <>
struct test_values<sib_information> {
  static std::vector<sib_information> get()
  {
    sib_information sib{};
    sib.si_indicator      = sib_information::sib1;
    sib.pdsch_cfg.bwp_cfg = &test_helper::make_test_schedtrace_cell_cfg().init_dl_bwp;
    sib.pdsch_cfg.rbs     = vrb_alloc(vrb_interval{0, 0});
    sib.pdsch_cfg.symbols = ofdm_symbol_range{0, 1};
    pdsch_codeword cw{};
    cw.rv_index      = 0;
    cw.mcs_index     = sch_mcs_index{0};
    cw.tb_size_bytes = units::bytes{0};
    sib.pdsch_cfg.codewords.push_back(cw);
    return field_variations_from(sib);
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
