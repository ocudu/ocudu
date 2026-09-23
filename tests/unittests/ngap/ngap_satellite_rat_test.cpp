// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ngap/ngap_asn1_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

supported_tracking_area make_supported_ta(tac_t tac, std::optional<satellite_rat_type> rat)
{
  return {tac, {{plmn_identity::test_value(), {s_nssai_t{slice_service_type{1}}}}}, rat};
}

/// Fills an NG Setup Request for the given tracking areas, and returns it as decoded by the AMF.
asn1::ngap::ng_setup_request_s pack_and_unpack_ng_setup_request(std::vector<supported_tracking_area> tas)
{
  ngap_context_t ctxt = {.gnb_id = {411, 22}, .ran_node_name = "tstgnb01", .supported_tas = std::move(tas)};

  asn1::ngap::ngap_pdu_c pdu;
  pdu.set_init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
  fill_asn1_ng_setup_request(pdu.init_msg().value.ng_setup_request(), ctxt);

  byte_buffer buf;
  {
    asn1::bit_ref bref{buf};
    EXPECT_EQ(pdu.pack(bref), asn1::OCUDUASN_SUCCESS);
  }
  asn1::ngap::ngap_pdu_c decoded;
  {
    asn1::cbit_ref bref{buf};
    EXPECT_EQ(decoded.unpack(bref), asn1::OCUDUASN_SUCCESS);
  }
  return decoded.init_msg().value.ng_setup_request();
}

} // namespace

TEST(ngap_satellite_rat_test, terrestrial_tracking_area_carries_no_rat_information)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-5");

  asn1::ngap::ng_setup_request_s req = pack_and_unpack_ng_setup_request({make_supported_ta(7, std::nullopt)});

  ASSERT_EQ(req->supported_ta_list.size(), 1);
  ASSERT_FALSE(req->supported_ta_list[0].ie_exts_present);
}

TEST(ngap_satellite_rat_test, ntn_tracking_area_carries_its_satellite_rat_type)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-5");

  // TS 38.413, Section 9.3.1.125.
  const std::array<std::pair<satellite_rat_type, asn1::ngap::rat_info_opts::options>, 4> rats = {
      {{satellite_rat_type::nr_leo, asn1::ngap::rat_info_opts::nr_leo},
       {satellite_rat_type::nr_meo, asn1::ngap::rat_info_opts::nr_meo},
       {satellite_rat_type::nr_geo, asn1::ngap::rat_info_opts::nr_geo},
       {satellite_rat_type::nr_othersat, asn1::ngap::rat_info_opts::nr_othersat}}};

  for (const auto& [rat, asn1_rat] : rats) {
    asn1::ngap::ng_setup_request_s req = pack_and_unpack_ng_setup_request({make_supported_ta(7, rat)});

    ASSERT_EQ(req->supported_ta_list.size(), 1);
    const asn1::ngap::supported_ta_item_s& ta = req->supported_ta_list[0];
    ASSERT_TRUE(ta.ie_exts_present);
    ASSERT_TRUE(ta.ie_exts.rat_info_present);
    ASSERT_EQ(ta.ie_exts.rat_info.value, asn1_rat);
  }
}

TEST(ngap_satellite_rat_test, satellite_rat_type_is_signalled_per_tracking_area)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-5");

  asn1::ngap::ng_setup_request_s req = pack_and_unpack_ng_setup_request(
      {make_supported_ta(7, satellite_rat_type::nr_geo), make_supported_ta(8, std::nullopt)});

  ASSERT_EQ(req->supported_ta_list.size(), 2);
  ASSERT_TRUE(req->supported_ta_list[0].ie_exts.rat_info_present);
  ASSERT_EQ(req->supported_ta_list[0].ie_exts.rat_info.value, asn1::ngap::rat_info_opts::nr_geo);
  ASSERT_FALSE(req->supported_ta_list[1].ie_exts_present);
}
