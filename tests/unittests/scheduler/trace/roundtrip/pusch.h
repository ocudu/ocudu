// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/scheduler/result/pusch_info.h"
#include "ocudu/scheduler/result/vrb_alloc.h"
#include <gtest/gtest.h>

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<vrb_alloc> {
  static std::vector<vrb_alloc> get()
  {
    constexpr uint16_t max_vrb = std::numeric_limits<uint16_t>::max();
    rbg_bitmap         rbgs(4);
    rbgs.from_uint64(0b1001);
    return {vrb_alloc(vrb_interval{0, 0}), vrb_alloc(vrb_interval{0, max_vrb}), vrb_alloc(rbgs)};
  }
};

// The wire carries intra_slot_freq_hopping and pusch_second_hop_prb together, as the optional
// fbs::VrbAlloc::vrb_second_hop_start, and only for a type-1 (contiguous VRB) allocation -- intra-slot frequency
// hopping is only defined for resource allocation type 1, as per TS38.214 Section 6.3. Hence both are compare-only:
// a sweep would set each in isolation (a second hop PRB with hopping disabled is not encoded) and would carry
// hopping enabled forward into the type-0 instance test_values<vrb_alloc> ends on. test_values<pusch_information>
// below appends the valid combinations instead.
template <>
struct roundtrip_traits<pusch_information> {
  using pusch = pusch_information;
  static constexpr auto members =
      std::make_tuple(field("bwp_cfg", &pusch::bwp_cfg),
                      field("rnti", &pusch::rnti),
                      field("rbs", &pusch::rbs),
                      field("symbols", &pusch::symbols),
                      compare_only_field("intra_slot_freq_hopping", &pusch::intra_slot_freq_hopping),
                      compare_only_field("pusch_second_hop_prb", &pusch::pusch_second_hop_prb),
                      field("rv_index", &pusch::rv_index),
                      field("nof_layers", &pusch::nof_layers),
                      field("harq_id", &pusch::harq_id),
                      field("tb_size_bytes", &pusch::tb_size_bytes));
};

// bwp_cfg is not swept independently (it has no test_values<const bwp_configuration*>) but is still compared on
// every roundtrip check.
template <>
struct test_values<pusch_information> {
  static std::vector<pusch_information> get()
  {
    pusch_information base{};
    base.bwp_cfg                          = &test_helper::make_test_schedtrace_cell_cfg().init_ul_bwp;
    std::vector<pusch_information> values = field_variations_from(base);

    // Intra-slot frequency hopping, at both extremes of the second hop PRB the wire can represent. Type-1 only, see
    // roundtrip_traits<pusch_information> above.
    for (const uint16_t second_hop_prb : {uint16_t{0}, std::numeric_limits<uint16_t>::max()}) {
      pusch_information hopping       = base;
      hopping.rbs                     = vrb_alloc(vrb_interval{0, 10});
      hopping.intra_slot_freq_hopping = true;
      hopping.pusch_second_hop_prb    = second_hop_prb;
      values.push_back(hopping);
    }
    return values;
  }
};

template <>
struct roundtrip_traits<ul_sched_info::decision_context> {
  static constexpr auto members = std::make_tuple(field("ue_index", &ul_sched_info::decision_context::ue_index),
                                                  field("nof_retxs", &ul_sched_info::decision_context::nof_retxs),
                                                  field("k2", &ul_sched_info::decision_context::k2));
};

template <>
struct roundtrip_traits<ul_sched_info> {
  static constexpr auto members =
      std::make_tuple(field("pusch_cfg", &ul_sched_info::pusch_cfg), field("context", &ul_sched_info::context));

  static ul_sched_info roundtrip(const ul_sched_info& pusch)
  {
    return roundtrip_via<fbs::Pusch>(pusch, convert_pusch_to_fb, [](ul_sched_info& out, const fbs::Pusch& fb) {
      const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
      convert_fb_to_pusch(out, fb, &cell_cfg.init_ul_bwp);
    });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
