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

/// Converts the given MAC PRACH occasion into a FAPI PRACH PDU.
static fapi::ul_prach_pdu convert_prach(const prach_occasion_info& mac_pdu)
{
  fapi::ul_prach_pdu         fapi_pdu;
  fapi::ul_prach_pdu_builder builder(fapi_pdu);
  convert_prach_mac_to_fapi(builder, mac_pdu);
  return fapi_pdu;
}

TEST(mac_fapi_ul_prach_pdu_conversor_test, valid_prach_pdu_should_pass)
{
  prach_occasion_info mac_pdu = build_valid_prach_occasion();

  fapi::ul_prach_pdu fapi_pdu = convert_prach(mac_pdu);

  EXPECT_EQ(mac_pdu.format, fapi_pdu.prach_format);
  EXPECT_EQ(is_long_preamble(mac_pdu.format) ? 1 : mac_pdu.nof_prach_occasions, fapi_pdu.num_prach_ocas);
  EXPECT_EQ(mac_pdu.index_fd_ra, fapi_pdu.index_fd_ra);
  EXPECT_EQ(mac_pdu.nof_fd_ra, fapi_pdu.num_fd_ra);
  EXPECT_EQ(mac_pdu.start_symbol, fapi_pdu.prach_start_symbol);
  EXPECT_EQ(mac_pdu.nof_cs, fapi_pdu.num_cs);
  EXPECT_EQ(mac_pdu.start_preamble_index, fapi_pdu.preambles.start());
  EXPECT_EQ(mac_pdu.nof_preamble_indexes, fapi_pdu.preambles.length());
}

namespace {

/// PRACH format and the number of time-domain PRACH occasions that FAPI must report for it.
struct prach_format_param {
  prach_format_type format;
  unsigned          expected_nof_occasions;
};

class mac_fapi_ul_prach_format_fixture : public ::testing::TestWithParam<prach_format_param>
{};

} // namespace

TEST_P(mac_fapi_ul_prach_format_fixture, nof_time_domain_occasions_depends_on_the_preamble_length)
{
  static constexpr unsigned nof_prach_occasions = 5;

  prach_occasion_info mac_pdu = build_valid_prach_occasion();
  mac_pdu.format              = GetParam().format;
  mac_pdu.nof_prach_occasions = nof_prach_occasions;

  fapi::ul_prach_pdu fapi_pdu = convert_prach(mac_pdu);

  EXPECT_EQ(GetParam().format, fapi_pdu.prach_format);
  EXPECT_EQ(GetParam().expected_nof_occasions, fapi_pdu.num_prach_ocas);
}

// Long preambles do not use the time-domain occasions, so FAPI requires a single one. Short preambles report them all.
INSTANTIATE_TEST_SUITE_P(
    prach_formats,
    mac_fapi_ul_prach_format_fixture,
    ::testing::Values(prach_format_param{.format = prach_format_type::zero, .expected_nof_occasions = 1},
                      prach_format_param{.format = prach_format_type::one, .expected_nof_occasions = 1},
                      prach_format_param{.format = prach_format_type::two, .expected_nof_occasions = 1},
                      prach_format_param{.format = prach_format_type::three, .expected_nof_occasions = 1},
                      prach_format_param{.format = prach_format_type::A1, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::A2, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::A3, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::B1, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::B4, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::C0, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::C2, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::A1_B1, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::A2_B2, .expected_nof_occasions = 5},
                      prach_format_param{.format = prach_format_type::A3_B3, .expected_nof_occasions = 5}),
    [](const ::testing::TestParamInfo<prach_format_param>& param_info) {
      return fmt::format("format_{}", static_cast<unsigned>(param_info.param.format));
    });
