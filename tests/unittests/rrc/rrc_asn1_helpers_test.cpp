// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rrc/ue/rrc_asn1_converters.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/five_g_s_tmsi.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::ocucp;

/// Test five-g-s-tmsi conversion
TEST(rrc_asn1_helpers_test, test_five_g_s_tmsi_converter_for_valid_five_g_s_tmsi)
{
  // use known a Five-G-S-TMSI
  asn1::fixed_bitstring<48> asn1_five_g_s_tmsi;
  asn1_five_g_s_tmsi.from_number(278099133963U);

  five_g_s_tmsi_t five_g_s_tmsi = asn1_to_five_g_s_tmsi(asn1_five_g_s_tmsi);

  ASSERT_EQ(1U, five_g_s_tmsi.get_amf_set_id());
  ASSERT_EQ(0U, five_g_s_tmsi.get_amf_pointer());
  ASSERT_EQ(3221227019U, five_g_s_tmsi.get_five_g_tmsi());
}

/// Test five-g-s-tmsi conversion with concatenation
TEST(rrc_asn1_helpers_test, test_five_g_s_tmsi_concatenation_for_valid_five_g_s_tmsi)
{
  // use known Five-G-S-TMSI-Par1 and Five-G-S-TMSI-Part2
  asn1::fixed_bitstring<39> asn1_five_g_s_tmsi_part1;
  asn1_five_g_s_tmsi_part1.from_number(278099133963);

  asn1::fixed_bitstring<9> asn1_five_g_s_tmsi_part_2;
  asn1_five_g_s_tmsi_part_2.from_number(0);

  five_g_s_tmsi_t five_g_s_tmsi = asn1_to_five_g_s_tmsi(asn1_five_g_s_tmsi_part1, asn1_five_g_s_tmsi_part_2);

  ASSERT_EQ(1U, five_g_s_tmsi.get_amf_set_id());
  ASSERT_EQ(0U, five_g_s_tmsi.get_amf_pointer());
  ASSERT_EQ(3221227019U, five_g_s_tmsi.get_five_g_tmsi());
}

/// Test amf-identifier decoding
TEST(rrc_asn1_helpers_test, test_amf_identifier_converter_for_valid_amf_id)
{
  // use known a amf-identifier
  asn1::rrc_nr::registered_amf_s registered_amf;
  registered_amf.amf_id.from_number(0xf511b2);

  amf_identifier_t amf_id = asn1_to_amf_identifier(registered_amf.amf_id);

  ASSERT_EQ(245U, amf_id.amf_region_id);
  ASSERT_EQ(70U, amf_id.amf_set_id);
  ASSERT_EQ(50U, amf_id.amf_pointer);
}
