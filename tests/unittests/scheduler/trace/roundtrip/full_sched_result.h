// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file A sched_result with every serialized list populated, built from the roundtrip harness's own test values.

#pragma once

#include "csi_rs.h"               // IWYU pragma: keep
#include "dl_paging_allocation.h" // IWYU pragma: keep
#include "failed_attempts.h"      // IWYU pragma: keep
#include "pdcch.h"                // IWYU pragma: keep
#include "pdsch.h"                // IWYU pragma: keep
#include "prach.h"                // IWYU pragma: keep
#include "pucch_info.h"           // IWYU pragma: keep
#include "pusch.h"                // IWYU pragma: keep
#include "rar_information.h"      // IWYU pragma: keep
#include "roundtrip_test.h"
#include "sib_information.h" // IWYU pragma: keep
#include "srs.h"             // IWYU pragma: keep
#include "ssb.h"             // IWYU pragma: keep
#include "ocudu/scheduler/result/sched_result.h"

namespace ocudu::schedtrace::roundtrip_test {

/// \brief Builds a sched_result with every serialized list populated.
///
/// The elements are the roundtrip harness's own test values, so this stays in sync as wire fields are added: back() is
/// every field at its last corner value, front() at its first (used where a second element is worth having, to check a
/// list is refilled in order). Per-field coverage stays in each type's own roundtrip test; what this is for is the
/// aggregate -- that no list is encoded from, or decoded into, the wrong sched_result member.
inline sched_result make_full_sched_result()
{
  const cell_configuration& cell_cfg = test_helper::make_test_schedtrace_cell_cfg();

  sched_result result;
  result.success = true;

  result.ul.puschs.push_back(test_values<ul_sched_info>::get().front());
  result.ul.puschs.push_back(test_values<ul_sched_info>::get().back());
  // PUCCH is the one type built through its own helper rather than swept: test_values<pucch_info> varies res and
  // format_params as independent axes, so it also yields pairings that cannot occur (an F2 format_params on a Format 1
  // resource). A field-wise roundtrip does not care, but any consumer that correlates the two -- the scheduler's
  // result logger does, via std::get on the resource's format -- would throw. make_test_pucch keeps them consistent.
  result.ul.pucchs.emplace(test_helper::make_test_pucch(to_rnti(0x4603), cell_cfg.pucch_resources[0]));
  result.ul.pucchs.emplace(test_helper::make_test_pucch(
      to_rnti(0x4604), cell_cfg.pucch_resources[test_helper::find_test_pucch_res_idx(pucch_format::FORMAT_2)]));
  result.ul.prachs.push_back(test_values<prach_occasion_info>::get().back());
  result.ul.srss.push_back(test_values<srs_info>::get().back());

  result.dl.dl_pdcchs.push_back(test_values<pdcch_dl_information>::get().front());
  result.dl.dl_pdcchs.push_back(test_values<pdcch_dl_information>::get().back());
  result.dl.ul_pdcchs.push_back(test_values<pdcch_ul_information>::get().back());
  result.dl.bc.sibs.push_back(test_values<sib_information>::get().back());
  result.dl.bc.ssb_info.push_back(test_values<ssb_information>::get().back());
  result.dl.rar_grants.push_back(test_values<rar_information>::get().back());
  result.dl.paging_grants.push_back(test_values<dl_paging_allocation>::get().back());
  result.dl.ue_grants.push_back(test_values<dl_msg_alloc>::get().back());
  result.dl.csi_rs.push_back(test_values<csi_rs_info>::get().back());

  result.failed_attempts = test_values<failed_alloc_attempts>::get().back();

  return result;
}

} // namespace ocudu::schedtrace::roundtrip_test
