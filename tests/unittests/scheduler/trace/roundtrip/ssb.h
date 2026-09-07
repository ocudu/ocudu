// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/result/pdsch_info.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct roundtrip_traits<ssb_information> {
  static constexpr auto members = std::make_tuple(field("ssb_index", &ssb_information::ssb_index),
                                                  field("crbs", &ssb_information::crbs),
                                                  field("symbols", &ssb_information::symbols));

  static ssb_information roundtrip(const ssb_information& ssb)
  {
    ssb_information result{};
    convert_fb_to_ssb(result, convert_ssb_to_fb(ssb));
    return result;
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
