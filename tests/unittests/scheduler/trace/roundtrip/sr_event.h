// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/input/uci_inputs.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct roundtrip_traits<sr_event> {
  static constexpr auto members =
      std::make_tuple(field("ue_index", &sr_event::ue_index), field("rnti", &sr_event::rnti));

  static sr_event roundtrip(const sr_event& event)
  {
    return roundtrip_via<fbs::SrEvent>(event, convert_sr_event_to_fb, [](sr_event& result, const fbs::SrEvent& fb) {
      convert_fb_to_sr_event(result, fb);
    });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
