// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bwp_configuration.h"    // IWYU pragma: keep
#include "cell_configuration.h"   // IWYU pragma: keep
#include "csi_report_event.h"     // IWYU pragma: keep
#include "csi_rs.h"               // IWYU pragma: keep
#include "dl_paging_allocation.h" // IWYU pragma: keep
#include "failed_attempts.h"      // IWYU pragma: keep
#include "harq_ack_event.h"       // IWYU pragma: keep
#include "pdcch.h"                // IWYU pragma: keep
#include "pdsch.h"                // IWYU pragma: keep
#include "prach.h"                // IWYU pragma: keep
#include "pucch_info.h"           // IWYU pragma: keep
#include "pucch_resource.h"       // IWYU pragma: keep
#include "pusch.h"                // IWYU pragma: keep
#include "rach_indication.h"      // IWYU pragma: keep
#include "rar_information.h"      // IWYU pragma: keep
#include "sib_information.h"      // IWYU pragma: keep
#include "sr_event.h"             // IWYU pragma: keep
#include "srs.h"                  // IWYU pragma: keep
#include "ssb.h"                  // IWYU pragma: keep
#include <gtest/gtest.h>

namespace {

using namespace ocudu;
using namespace schedtrace;
using namespace schedtrace::roundtrip_test;

TEST(roundtrip_test, harq_ack_event)
{
  test_roundtrip<harq_ack_event>();
}

TEST(roundtrip_test, pucch_resource)
{
  test_roundtrip<pucch_resource>();
}

TEST(roundtrip_test, pucch_info)
{
  test_roundtrip<pucch_info>();
}

TEST(roundtrip_test, bwp_configuration)
{
  test_roundtrip<bwp_configuration>();
}

TEST(roundtrip_test, cell_configuration)
{
  test_roundtrip<schedtrace::cell_configuration>();
}

TEST(roundtrip_test, rach_indication)
{
  test_roundtrip<rach_indication_message>();
}

TEST(roundtrip_test, sr_event)
{
  test_roundtrip<sr_event>();
}

TEST(roundtrip_test, csi_report_event)
{
  test_roundtrip<csi_report_event>();
}

TEST(roundtrip_test, pusch)
{
  test_roundtrip<ul_sched_info>();
}

TEST(roundtrip_test, pdsch)
{
  test_roundtrip<dl_msg_alloc>();
}

TEST(roundtrip_test, sib_information)
{
  test_roundtrip<sib_information>();
}

TEST(roundtrip_test, rar_information)
{
  test_roundtrip<rar_information>();
}

TEST(roundtrip_test, dl_paging_allocation)
{
  test_roundtrip<dl_paging_allocation>();
}

TEST(roundtrip_test, srs)
{
  test_roundtrip<srs_info>();
}

TEST(roundtrip_test, csi_rs)
{
  test_roundtrip<csi_rs_info>();
}

TEST(roundtrip_test, failed_attempts)
{
  test_roundtrip<failed_alloc_attempts>();
}

TEST(roundtrip_test, ssb_information)
{
  test_roundtrip<ssb_information>();
}

TEST(roundtrip_test, prach)
{
  test_roundtrip<prach_occasion_info>();
}

TEST(roundtrip_test, dl_pdcch)
{
  test_roundtrip<pdcch_dl_information>();
}

TEST(roundtrip_test, ul_pdcch)
{
  test_roundtrip<pdcch_ul_information>();
}

} // namespace
