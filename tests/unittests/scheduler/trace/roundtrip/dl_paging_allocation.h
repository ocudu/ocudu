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
struct test_values<paging_ue_info::paging_identity_type> {
  static std::vector<paging_ue_info::paging_identity_type> get()
  {
    return {paging_ue_info::ran_ue_paging_identity, paging_ue_info::cn_ue_paging_identity};
  }
};

template <>
struct roundtrip_traits<paging_ue_info> {
  static constexpr auto members =
      std::make_tuple(field("paging_type_indicator", &paging_ue_info::paging_type_indicator),
                      field("paging_identity", &paging_ue_info::paging_identity));
};

// fbs::PagingPdsch carries only rbs/symbols/one codeword -- rnti/harq_id etc. are UE-grant-only.
template <>
struct roundtrip_traits<dl_paging_allocation> {
  static constexpr auto members = std::make_tuple(
      field("paging_ue_list", &dl_paging_allocation::paging_ue_list),
      computed_field<dl_paging_allocation, const bwp_configuration*>(
          "bwp_cfg",
          [](dl_paging_allocation& p) -> const bwp_configuration*& { return p.pdsch_cfg.bwp_cfg; }),
      computed_field<dl_paging_allocation, vrb_alloc>(
          "rbs",
          [](dl_paging_allocation& p) -> vrb_alloc& { return p.pdsch_cfg.rbs; }),
      computed_field<dl_paging_allocation, ofdm_symbol_range>(
          "symbols",
          [](dl_paging_allocation& p) -> ofdm_symbol_range& { return p.pdsch_cfg.symbols; }),
      computed_field<dl_paging_allocation, uint8_t>(
          "rv_index",
          [](dl_paging_allocation& p) -> uint8_t& { return p.pdsch_cfg.codewords[0].rv_index; }),
      computed_field<dl_paging_allocation, sch_mcs_index>(
          "mcs_index",
          [](dl_paging_allocation& p) -> sch_mcs_index& { return p.pdsch_cfg.codewords[0].mcs_index; }),
      computed_field<dl_paging_allocation, units::bytes>("tb_size_bytes", [](dl_paging_allocation& p) -> units::bytes& {
        return p.pdsch_cfg.codewords[0].tb_size_bytes;
      }));

  static dl_paging_allocation roundtrip(const dl_paging_allocation& paging)
  {
    return roundtrip_via<fbs::PagingPdsch>(
        paging, convert_paging_pdsch_to_fb, [](dl_paging_allocation& out, const fbs::PagingPdsch& fb) {
          const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
          convert_fb_to_paging_pdsch(out, fb, &cell_cfg.init_dl_bwp);
        });
  }
};

// Custom base with a codeword already pushed, since the composite auto-sweep would index codewords[0] before it exists.
template <>
struct test_values<dl_paging_allocation> {
  static std::vector<dl_paging_allocation> get()
  {
    dl_paging_allocation paging{};
    paging.pdsch_cfg.bwp_cfg = &test_helper::make_test_schedtrace_cell_cfg().init_dl_bwp;
    paging.pdsch_cfg.rbs     = vrb_alloc(vrb_interval{0, 0});
    paging.pdsch_cfg.symbols = ofdm_symbol_range{0, 1};
    pdsch_codeword cw{};
    cw.rv_index      = 0;
    cw.mcs_index     = sch_mcs_index{0};
    cw.tb_size_bytes = units::bytes{0};
    paging.pdsch_cfg.codewords.push_back(cw);
    return field_variations_from(paging);
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
