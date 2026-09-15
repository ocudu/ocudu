// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include "ocudu/lpp/reference_location.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Covers the coarse UE location exchange, TS 38.300 sec. 16.14.8: which cells it runs on, and what the
/// response does to the stored position.
class rrc_ue_coarse_location : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void TearDown() override { ocudulog::flush(); }

  /// Brings up a connected RRC UE on a cell of \c band, mapping one area to a TAC when \c with_mapping.
  void init_cell(nr_band band, bool with_mapping, bool with_security = true)
  {
    rrc_ue_test_cell_params cell_params;
    cell_params.bands = {band};
    if (with_mapping) {
      // Two adjoining areas, so that a position can move from one TAC to another.
      cell_params.location_mapping.location_areas = {{7, 50.0, 52.0, 14.0, 17.0}, {8, 52.0, 54.0, 14.0, 17.0}};
    }
    init(cell_params);

    // The RRC setup creates SRB1 and leaves the UE connected, which is what security activation and the request both
    // need.
    receive_setup_request();
    receive_setup_complete();
    if (with_security) {
      ASSERT_TRUE(init_security_context());
    }

    // Only the coarse location exchange is of interest, not the setup that brought the UE up.
    rrc_ue_f1ap_notifier.last_rrc_pdu.clear();
  }

  /// The UEInformationRequest sent on SRB1, which must be the last message the RRC UE sent.
  asn1::rrc_nr::ue_info_request_r16_s sent_request()
  {
    // What reaches the F1AP notifier is the PDCP PDU that carries the message.
    byte_buffer                 pdu = test_helpers::extract_dl_dcch_msg(get_srb1_pdu());
    asn1::cbit_ref              bref{pdu};
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
    EXPECT_EQ(dl_dcch_msg.unpack(bref), asn1::OCUDUASN_SUCCESS);
    EXPECT_EQ(dl_dcch_msg.msg.c1().type().value, asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types::ue_info_request_r16);

    return dl_dcch_msg.msg.c1().ue_info_request_r16();
  }

  /// Whether a UEInformationRequest asking for the coarse location was sent on SRB1.
  bool coarse_location_was_requested()
  {
    if (rrc_ue_f1ap_notifier.last_rrc_pdu.length() == 0) {
      return false;
    }

    // Held in a local: the IEs are a subobject of the message, which sent_request() returns by value.
    asn1::rrc_nr::ue_info_request_r16_s            request = sent_request();
    const asn1::rrc_nr::ue_info_request_r16_ies_s& ies     = request.crit_exts.ue_info_request_r16();
    return ies.non_crit_ext_present and ies.non_crit_ext.coarse_location_request_r17_present;
  }

  /// Feeds a UEInformationResponse back, carrying \c coarse_location_info as the coarseLocationInfo-r17 IE.
  void receive_ue_information_response(const std::vector<uint8_t>& coarse_location_info, bool with_v1700_ext = true)
  {
    asn1::rrc_nr::ul_dcch_msg_s       ul_dcch_msg;
    asn1::rrc_nr::ue_info_resp_r16_s& ue_info_resp =
        ul_dcch_msg.msg.set_msg_class_ext().set_c2().set_ue_info_resp_r16();
    ue_info_resp.rrc_transaction_id = sent_request().rrc_transaction_id;

    asn1::rrc_nr::ue_info_resp_r16_ies_s& ies = ue_info_resp.crit_exts.set_ue_info_resp_r16();
    if (with_v1700_ext) {
      ies.non_crit_ext_present = true;
      ies.non_crit_ext.coarse_location_info_r17.from_bytes(coarse_location_info);
    }

    byte_buffer   pdu;
    asn1::bit_ref bref{pdu};
    ASSERT_EQ(ul_dcch_msg.pack(bref), asn1::OCUDUASN_SUCCESS);

    rrc_ue->handle_ul_dcch_pdu(srb_id_t::srb1, std::move(pdu), /* integrity_verified */ true);
  }

  /// Brings up a cell worth asking on and sends the request.
  void init_and_request()
  {
    init_cell(nr_band::n256, /* with_mapping */ true);
    rrc_ue->request_coarse_ue_location();
    ASSERT_TRUE(coarse_location_was_requested());
  }

  /// The TAC the RRC UE derives for the stored position, as the NGAP reads it out of the UE.
  std::optional<tac_t> derived_tac()
  {
    cu_cp_user_location_info_nr user_location_info;
    rrc_ue->fill_ue_derived_location(user_location_info);
    return user_location_info.ue_location_derived_tac;
  }

  /// An Ellipsoid-Point of TS 37.355, the shape the UE reports its coarse location in.
  static std::vector<uint8_t> packed_position(const reference_location& loc)
  {
    byte_buffer packed = lpp::pack_reference_location(loc);
    return std::vector<uint8_t>(packed.begin(), packed.end());
  }
};

TEST_F(rrc_ue_coarse_location, ntn_cell_with_a_mapping_is_asked_for_the_coarse_location)
{
  init_cell(nr_band::n256, /* with_mapping */ true);

  rrc_ue->request_coarse_ue_location();

  EXPECT_TRUE(coarse_location_was_requested());
}

TEST_F(rrc_ue_coarse_location, ntn_cell_without_a_mapping_is_not_asked)
{
  // Nothing turns a position into a TAC in this cell, so the answer would have no use.
  init_cell(nr_band::n256, /* with_mapping */ false);

  rrc_ue->request_coarse_ue_location();

  EXPECT_FALSE(coarse_location_was_requested());
}

TEST_F(rrc_ue_coarse_location, terrestrial_cell_is_not_asked)
{
  // A TN cell does not span several tracking areas, whatever the configuration says.
  init_cell(nr_band::n78, /* with_mapping */ true);

  rrc_ue->request_coarse_ue_location();

  EXPECT_FALSE(coarse_location_was_requested());
}

TEST_F(rrc_ue_coarse_location, ue_that_left_connected_mode_is_not_asked)
{
  // The request is queued, so the UE may have been suspended by the time it runs.
  init_cell(nr_band::n256, /* with_mapping */ true);
  rrc_ue->set_rrc_state(rrc_state::inactive);

  rrc_ue->request_coarse_ue_location();

  EXPECT_FALSE(coarse_location_was_requested());
}

TEST_F(rrc_ue_coarse_location, ue_without_as_security_is_not_asked)
{
  // TS 38.331 sec. 5.7.10.2 allows the request only once AS security is active.
  init_cell(nr_band::n256, /* with_mapping */ true, /* with_security */ false);

  rrc_ue->request_coarse_ue_location();

  EXPECT_FALSE(coarse_location_was_requested());
}

TEST_F(rrc_ue_coarse_location, reported_position_is_stored_and_reported_to_the_cu_cp)
{
  init_and_request();

  receive_ue_information_response(packed_position({51.0, 15.0}));

  // The TAC of the area holding the position, which only a correctly decoded and stored one yields.
  EXPECT_EQ(derived_tac(), 7);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_location_updates, 1);
}

TEST_F(rrc_ue_coarse_location, a_position_that_did_not_move_is_reported_once)
{
  init_and_request();
  receive_ue_information_response(packed_position({51.0, 15.0}));

  // Asking again yields the same coordinates, which derive the same TAC.
  rrc_ue->request_coarse_ue_location();
  receive_ue_information_response(packed_position({51.0, 15.0}));

  EXPECT_EQ(derived_tac(), 7);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_location_updates, 1);
}

TEST_F(rrc_ue_coarse_location, a_position_that_moved_is_reported_again)
{
  init_and_request();
  receive_ue_information_response(packed_position({51.0, 15.0}));

  // The UE moved into the adjoining area, so the TAC the AMF is told changes with it.
  rrc_ue->request_coarse_ue_location();
  receive_ue_information_response(packed_position({53.0, 15.0}));

  EXPECT_EQ(derived_tac(), 8);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_location_updates, 2);
}

TEST_F(rrc_ue_coarse_location, response_without_the_v1700_extension_reports_nothing)
{
  // A UE that does not implement the r17 extension answers without it.
  init_and_request();

  receive_ue_information_response({}, /* with_v1700_ext */ false);

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_location_updates, 0);
}

TEST_F(rrc_ue_coarse_location, response_without_a_position_reports_nothing)
{
  // coarseLocationInfo has no presence flag, so "not available" arrives as an empty octet string.
  init_and_request();

  receive_ue_information_response({});

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_location_updates, 0);
}

TEST_F(rrc_ue_coarse_location, undecodable_position_reports_nothing)
{
  init_and_request();

  receive_ue_information_response({0xff, 0xff, 0xff});

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_location_updates, 0);
}
