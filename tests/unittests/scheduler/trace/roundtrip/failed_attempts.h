// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/result/sched_result.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct roundtrip_traits<failed_alloc_attempts> {
  static constexpr auto members =
      std::make_tuple(field("dl_pdcch", &failed_alloc_attempts::dl_pdcch),
                      field("ul_pdcch", &failed_alloc_attempts::ul_pdcch),
                      field("common_dl_pdcch", &failed_alloc_attempts::common_dl_pdcch),
                      field("common_ul_pdcch", &failed_alloc_attempts::common_ul_pdcch),
                      field("uci", &failed_alloc_attempts::uci),
                      field("fallback_uci_allocs", &failed_alloc_attempts::fallback_uci_allocs));

  static failed_alloc_attempts roundtrip(const failed_alloc_attempts& failed_attempts)
  {
    failed_alloc_attempts result{};
    convert_fb_to_failed_attempts(result, convert_failed_attempts_to_fb(failed_attempts));
    return result;
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
