// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/input/uci_inputs.h"
#include <gtest/gtest.h>

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<harq_id_t> {
  static std::vector<harq_id_t> get() { return {harq_id_t(0), harq_id_t::INVALID_HARQ_ID}; }
};

template <>
struct test_values<mac_harq_ack_report_status> {
  static std::vector<mac_harq_ack_report_status> get()
  {
    return {mac_harq_ack_report_status::nack, mac_harq_ack_report_status::ack, mac_harq_ack_report_status::dtx};
  }
};

template <>
struct roundtrip_traits<harq_ack_event> {
  static constexpr auto members = std::make_tuple(field("ue_index", &harq_ack_event::ue_index),
                                                  field("rnti", &harq_ack_event::rnti),
                                                  field("cell_index", &harq_ack_event::cell_index),
                                                  field("sl_ack_rx", &harq_ack_event::sl_ack_rx),
                                                  field("h_id", &harq_ack_event::h_id),
                                                  field("ack", &harq_ack_event::ack),
                                                  field("tbs", &harq_ack_event::tbs));

  static harq_ack_event roundtrip(const harq_ack_event& event)
  {
    return roundtrip_via<fbs::HarqAckEvent>(
        event, convert_harq_ack_event_to_fb, [&event](harq_ack_event& result, const fbs::HarqAckEvent& fb_event) {
          convert_fb_to_harq_ack_event(result, fb_event, event.sl_ack_rx.scs(), event.cell_index);
        });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
