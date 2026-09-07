// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/result/prach_info.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<prach_format_type> {
  static std::vector<prach_format_type> get()
  {
    return {prach_format_type::zero, prach_format_type::A1_B1, prach_format_type::invalid};
  }
};

template <>
struct roundtrip_traits<prach_occasion_info> {
  static constexpr auto members =
      std::make_tuple(field("pci", &prach_occasion_info::pci),
                      field("nof_prach_occasions", &prach_occasion_info::nof_prach_occasions),
                      field("format", &prach_occasion_info::format),
                      field("index_fd_ra", &prach_occasion_info::index_fd_ra),
                      field("start_symbol", &prach_occasion_info::start_symbol),
                      field("nof_cs", &prach_occasion_info::nof_cs),
                      field("nof_fd_ra", &prach_occasion_info::nof_fd_ra),
                      field("start_preamble_index", &prach_occasion_info::start_preamble_index),
                      field("nof_preamble_indexes", &prach_occasion_info::nof_preamble_indexes));

  static prach_occasion_info roundtrip(const prach_occasion_info& prach)
  {
    prach_occasion_info result{};
    convert_fb_to_prach(result, convert_prach_to_fb(prach));
    return result;
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
