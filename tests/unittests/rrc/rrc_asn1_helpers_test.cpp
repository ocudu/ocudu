// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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

/// Test that a UE capability RAT container list survives a conversion to the common type and back.
TEST(rrc_asn1_helpers_test, test_ue_cap_rat_container_list_converter_round_trip)
{
  asn1::rrc_nr::ue_cap_rat_container_list_l asn1_capabilities_list;
  asn1_capabilities_list.resize(2);
  asn1_capabilities_list[0].rat_type.value = asn1::rrc_nr::rat_type_opts::options::nr;
  ASSERT_TRUE(asn1_capabilities_list[0].ue_cap_rat_container.append(std::array<uint8_t, 3>{0x11, 0x22, 0x33}));
  asn1_capabilities_list[1].rat_type.value = asn1::rrc_nr::rat_type_opts::options::eutra;
  ASSERT_TRUE(asn1_capabilities_list[1].ue_cap_rat_container.append(std::array<uint8_t, 2>{0xaa, 0xbb}));

  // Convert to the common type.
  rrc_ue_cap_rat_container_list_t capabilities_list = asn1_to_ue_cap_rat_container_list(asn1_capabilities_list);

  ASSERT_EQ(2U, capabilities_list.size());
  ASSERT_EQ(rat_type_t::nr, capabilities_list[0].rat_type);
  ASSERT_EQ(asn1_capabilities_list[0].ue_cap_rat_container, capabilities_list[0].ue_cap_rat_container);
  ASSERT_EQ(rat_type_t::eutra, capabilities_list[1].rat_type);
  ASSERT_EQ(asn1_capabilities_list[1].ue_cap_rat_container, capabilities_list[1].ue_cap_rat_container);

  // Convert back to ASN.1.
  asn1::rrc_nr::ue_cap_rat_container_list_l asn1_result = ue_cap_rat_container_list_to_asn1(capabilities_list);

  ASSERT_EQ(asn1_capabilities_list.size(), asn1_result.size());
  for (unsigned i = 0, e = asn1_capabilities_list.size(); i != e; ++i) {
    ASSERT_EQ(asn1_capabilities_list[i].rat_type, asn1_result[i].rat_type);
    ASSERT_EQ(asn1_capabilities_list[i].ue_cap_rat_container, asn1_result[i].ue_cap_rat_container);
  }
}

/// Test that the common UE capability RAT container list never exceeds the ASN.1 list bound.
TEST(rrc_asn1_helpers_test, test_ue_cap_rat_container_list_converter_respects_capacity)
{
  asn1::rrc_nr::ue_cap_rat_container_list_l asn1_capabilities_list;
  asn1_capabilities_list.resize(MAX_NOF_UE_CAP_RAT_CONTAINERS + 2);
  for (auto& asn1_container : asn1_capabilities_list) {
    asn1_container.rat_type.value = asn1::rrc_nr::rat_type_opts::options::nr;
  }

  rrc_ue_cap_rat_container_list_t capabilities_list = asn1_to_ue_cap_rat_container_list(asn1_capabilities_list);

  ASSERT_EQ(MAX_NOF_UE_CAP_RAT_CONTAINERS, capabilities_list.size());
}
