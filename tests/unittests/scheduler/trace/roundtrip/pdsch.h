// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include <gtest/gtest.h>

namespace ocudu::schedtrace::roundtrip_test {

// mcs_descr is not serialized -- it is derivable from mcs_index and mcs_table.
template <>
struct roundtrip_traits<pdsch_codeword> {
  static constexpr auto members = std::make_tuple(field("rv_index", &pdsch_codeword::rv_index),
                                                  field("mcs_index", &pdsch_codeword::mcs_index),
                                                  field("tb_size_bytes", &pdsch_codeword::tb_size_bytes),
                                                  field("new_data", &pdsch_codeword::new_data));
};

// coreset_cfg is not part of the wire format -- fbs::Pdsch carries no coreset index to look it up by, so
// convert_fb_to_pdsch always leaves it null; it is excluded here rather than compared against garbage.
template <>
struct roundtrip_traits<pdsch_information> {
  static constexpr auto members = std::make_tuple(field("bwp_cfg", &pdsch_information::bwp_cfg),
                                                  field("rnti", &pdsch_information::rnti),
                                                  field("rbs", &pdsch_information::rbs),
                                                  field("symbols", &pdsch_information::symbols),
                                                  field("codewords", &pdsch_information::codewords),
                                                  field("harq_id", &pdsch_information::harq_id));
};

// bwp_cfg is not swept independently (it has no test_values<const bwp_configuration*>) but is still compared on
// every roundtrip check.
template <>
struct test_values<pdsch_information> {
  static std::vector<pdsch_information> get()
  {
    pdsch_information base{};
    base.bwp_cfg = &test_helper::make_test_schedtrace_cell_cfg().init_dl_bwp;
    return field_variations_from(base);
  }
};

template <>
struct roundtrip_traits<dl_msg_alloc::decision_context> {
  static constexpr auto members = std::make_tuple(field("ue_index", &dl_msg_alloc::decision_context::ue_index),
                                                  field("nof_retxs", &dl_msg_alloc::decision_context::nof_retxs),
                                                  field("k1", &dl_msg_alloc::decision_context::k1));
};

template <>
struct roundtrip_traits<dl_msg_alloc> {
  static constexpr auto members =
      std::make_tuple(field("pdsch_cfg", &dl_msg_alloc::pdsch_cfg), field("context", &dl_msg_alloc::context));

  static dl_msg_alloc roundtrip(const dl_msg_alloc& pdsch)
  {
    return roundtrip_via<fbs::Pdsch>(pdsch, convert_pdsch_to_fb, [](dl_msg_alloc& out, const fbs::Pdsch& fb) {
      const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();
      convert_fb_to_pdsch(out, fb, &cell_cfg.init_dl_bwp);
    });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
