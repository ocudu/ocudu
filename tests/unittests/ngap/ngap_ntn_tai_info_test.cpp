// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ngap/ngap_asn1_converters.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

/// Uu Cell ID of the serving cell, which the gNB reports unless a Mapped Cell ID applies.
constexpr uint64_t uu_nci = 0x19b0;

cu_cp_user_location_info_nr make_uli(std::initializer_list<tac_t> broadcast_tacs)
{
  cu_cp_user_location_info_nr uli;
  uli.nr_cgi.plmn_id = plmn_identity::test_value();
  uli.nr_cgi.nci     = nr_cell_identity::create(uu_nci).value();
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

TEST(ngap_ntn_tai_info_test, ue_location_derived_tac_is_absent_without_a_coarse_ue_location)
{
  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(make_uli({7, 8, 9}));

  EXPECT_FALSE(asn1_uli.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn_present);
}

TEST(ngap_ntn_tai_info_test, ue_location_derived_tac_is_reported_when_it_was_derived)
{
  cu_cp_user_location_info_nr uli = make_uli({7, 8, 9});
  uli.ue_location_derived_tac     = 8;

  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(uli);

  ASSERT_TRUE(asn1_uli.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn_present);
  EXPECT_EQ(asn1_uli.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn.to_number(), 8);

  // The derived TAC is reported alongside the broadcast list, not instead of it.
  EXPECT_EQ(asn1_uli.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn.size(), 3);
  EXPECT_EQ(asn1_uli.tai.tac.to_number(), 7);
}

TEST(ngap_ntn_tai_info_test, single_tac_ntn_cell_reports_ntn_tai_information_once_a_tac_is_derived)
{
  // TS 38.300 sec. 16.14.3.1 leaves broadcasting several TACs optional, so an NTN cell may broadcast one TAC and still
  // report the TAC derived from the UE location, sec. 16.14.5. A derived TAC only exists where a location mapping is
  // configured, which no terrestrial cell has, so this state is reached by an NTN cell alone.
  cu_cp_user_location_info_nr uli = make_uli({});
  uli.ue_location_derived_tac     = 9;

  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(uli);

  ASSERT_TRUE(asn1_uli.ie_exts.nr_ntn_tai_info_present);
  ASSERT_TRUE(asn1_uli.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn_present);
  EXPECT_EQ(asn1_uli.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn.to_number(), 9);

  // TS 38.413 sec. 9.3.3.53 takes at least one TAC, which is the single TAC the cell broadcasts.
  ASSERT_EQ(asn1_uli.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn.size(), 1);
  EXPECT_EQ(asn1_uli.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn[0].to_number(), 7);
}

TEST(ngap_ntn_tai_info_test, mapped_cell_id_replaces_the_uu_cell_id_of_the_reported_cgi)
{
  cu_cp_user_location_info_nr uli = make_uli({7, 8, 9});
  uli.mapped_nci                  = nr_cell_identity::create(0x66c001).value();

  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(uli);

  EXPECT_EQ(asn1_uli.nr_cgi.nr_cell_id.to_number(), 0x66c001);
}

TEST(ngap_ntn_tai_info_test, uu_cell_id_is_reported_without_a_mapped_cell_id)
{
  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(make_uli({7, 8, 9}));

  // TS 38.300 sec. 16.14.5 constructs the Mapped Cell ID from the UE location, so an unknown location keeps the Uu
  // Cell ID of the serving cell.
  EXPECT_EQ(asn1_uli.nr_cgi.nr_cell_id.to_number(), uu_nci);
}

TEST(ngap_ntn_tai_info_test, mapped_cell_id_does_not_change_the_plmn_of_the_reported_cgi)
{
  cu_cp_user_location_info_nr uli = make_uli({7, 8, 9});
  uli.mapped_nci                  = nr_cell_identity::create(0x66c001).value();

  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(uli);

  // A Mapped Cell ID names a geographical area, and TS 38.300 sec. 16.14.5 NOTE 3 lets it stand for one outside the
  // serving PLMN's country. It is still reported by this gNB for its own PLMN, so only the cell identity is replaced.
  const std::array<uint8_t, 3> expected_plmn = plmn_identity::test_value().to_bytes();
  for (unsigned i = 0; i != expected_plmn.size(); ++i) {
    EXPECT_EQ(asn1_uli.nr_cgi.plmn_id[i], expected_plmn[i]) << "NR CGI PLMN octet " << i;
  }
}

TEST(ngap_ntn_tai_info_test, ntn_tai_information_survives_a_pack_unpack_round_trip)
{
  cu_cp_user_location_info_nr uli = make_uli({7, 8, 9});
  uli.ue_location_derived_tac     = 8;

  const asn1::ngap::user_location_info_nr_s asn1_uli = cu_cp_user_location_info_to_asn1(uli);

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  ASSERT_EQ(asn1_uli.pack(bref), asn1::OCUDUASN_SUCCESS);

  asn1::cbit_ref                      bref2{buf};
  asn1::ngap::user_location_info_nr_s unpacked;
  ASSERT_EQ(unpacked.unpack(bref2), asn1::OCUDUASN_SUCCESS);

  ASSERT_TRUE(unpacked.ie_exts.nr_ntn_tai_info_present) << "NR NTN TAI Information did not survive the round trip";
  ASSERT_EQ(unpacked.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn.size(), 3);
  EXPECT_EQ(unpacked.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn[2].to_number(), 9);
  ASSERT_TRUE(unpacked.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn_present);
  EXPECT_EQ(unpacked.ie_exts.nr_ntn_tai_info.ue_location_derived_tac_in_nr_ntn.to_number(), 8);
}
