// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/ran/bwp/bwp_configuration.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<cyclic_prefix> {
  static std::vector<cyclic_prefix> get() { return {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}; }
};

template <>
struct test_values<subcarrier_spacing> {
  static std::vector<subcarrier_spacing> get()
  {
    return {subcarrier_spacing::kHz15,
            subcarrier_spacing::kHz30,
            subcarrier_spacing::kHz60,
            subcarrier_spacing::kHz120,
            subcarrier_spacing::kHz240};
  }
};

template <>
struct roundtrip_traits<bwp_configuration> {
  static constexpr auto members = std::make_tuple(field("cp", &bwp_configuration::cp),
                                                  field("scs", &bwp_configuration::scs),
                                                  field("crbs", &bwp_configuration::crbs));

  static bwp_configuration roundtrip(const bwp_configuration& bwp)
  {
    bwp_configuration result{};
    convert_fb_to_bwp_cfg(result, convert_bwp_cfg_to_fb(bwp));
    return result;
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
