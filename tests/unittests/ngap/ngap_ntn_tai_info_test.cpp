// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ngap/ngap_asn1_converters.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

cu_cp_user_location_info_nr make_uli(std::initializer_list<tac_t> broadcast_tacs)
{
  cu_cp_user_location_info_nr uli;
  uli.nr_cgi.plmn_id = plmn_identity::test_value();
  uli.nr_cgi.nci     = nr_cell_identity::create(6576).value();
  uli.tai            = {plmn_identity::test_value(), 7};
  for (tac_t tac : broadcast_tacs) {
    uli.tac_list.push_back(tac);
  }
  return uli;
}

} // namespace

TEST(ngap_ntn_tai_info_test, terrestrial_cell_reports_no_ntn_tai_information)
{
  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(make_uli({}));

  EXPECT_FALSE(asn1_uli.ie_exts.nr_ntn_tai_info_present);
  EXPECT_EQ(asn1_uli.tai.tac.to_number(), 7);
}

TEST(ngap_ntn_tai_info_test, multi_tac_cell_reports_every_broadcast_tac)
{
  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(make_uli({7, 8, 9}));

  ASSERT_TRUE(asn1_uli.ie_exts_present);
  ASSERT_TRUE(asn1_uli.ie_exts.nr_ntn_tai_info_present);

  const auto& ntn_tai_info = asn1_uli.ie_exts.nr_ntn_tai_info;

  const std::array<uint8_t, 3> expected_plmn = plmn_identity::test_value().to_bytes();
  for (unsigned i = 0; i != expected_plmn.size(); ++i) {
    EXPECT_EQ(ntn_tai_info.serving_plmn[i], expected_plmn[i]) << "serving PLMN octet " << i;
  }

  ASSERT_EQ(ntn_tai_info.tac_list_in_nr_ntn.size(), 3);
  EXPECT_EQ(ntn_tai_info.tac_list_in_nr_ntn[0].to_number(), 7);
  EXPECT_EQ(ntn_tai_info.tac_list_in_nr_ntn[1].to_number(), 8);
  EXPECT_EQ(ntn_tai_info.tac_list_in_nr_ntn[2].to_number(), 9);

  // The TAI keeps carrying the primary TAC, so an AMF ignoring the extension is unaffected.
  EXPECT_EQ(asn1_uli.tai.tac.to_number(), 7);
}

TEST(ngap_ntn_tai_info_test, ue_location_derived_tac_is_absent)
{
  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(make_uli({7, 8, 9}));

  // Deriving it needs the coarse UE location, which the gNB does not request yet.
  EXPECT_FALSE(asn1_uli.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn_present);
}

TEST(ngap_ntn_tai_info_test, ntn_tai_information_survives_a_pack_unpack_round_trip)
{
  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(make_uli({7, 8, 9}));

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  ASSERT_EQ(asn1_uli.pack(bref), asn1::OCUDUASN_SUCCESS);

  asn1::cbit_ref                      bref2{buf};
  asn1::ngap::user_location_info_nr_s unpacked;
  ASSERT_EQ(unpacked.unpack(bref2), asn1::OCUDUASN_SUCCESS);

  ASSERT_TRUE(unpacked.ie_exts.nr_ntn_tai_info_present) << "NR NTN TAI Information did not survive the round trip";
  ASSERT_EQ(unpacked.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn.size(), 3);
  EXPECT_EQ(unpacked.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn[2].to_number(), 9);
}
