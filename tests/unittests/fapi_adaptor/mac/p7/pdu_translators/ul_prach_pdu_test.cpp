// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "prach.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_ul_prach_pdu_conversor_test, valid_prach_pdu_should_pass)
{
  const prach_occasion_info& mac_pdu = build_valid_prach_occassion();
  fapi::ul_prach_pdu         fapi_pdu;
  fapi::ul_prach_pdu_builder builder(fapi_pdu);

  convert_prach_mac_to_fapi(builder, mac_pdu);

  ASSERT_EQ(static_cast<unsigned>(prach_format_type::one), static_cast<unsigned>(fapi_pdu.prach_format));
  ASSERT_EQ(is_long_preamble(mac_pdu.format) ? 1 : mac_pdu.nof_prach_occasions, fapi_pdu.num_prach_ocas);
  ASSERT_EQ(mac_pdu.index_fd_ra, fapi_pdu.index_fd_ra);
  ASSERT_EQ(mac_pdu.nof_fd_ra, fapi_pdu.num_fd_ra);
  ASSERT_EQ(mac_pdu.start_symbol, fapi_pdu.prach_start_symbol);
  ASSERT_EQ(mac_pdu.nof_cs, fapi_pdu.num_cs);
  ASSERT_EQ(mac_pdu.start_preamble_index, fapi_pdu.preambles.start());
  ASSERT_EQ(mac_pdu.nof_preamble_indexes, fapi_pdu.preambles.length());
}
