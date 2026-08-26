// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_packed_test_messages.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ran/tac.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// \brief Verifies that the TAC list a cell broadcasts reaches the AMF in the NR NTN TAI Information IE, TS 38.413.
///
/// The IE is filled at every site that builds User Location Information. This exercises the Initial UE Message path,
/// which is where every UE starts and which is fed by the RRC layer rather than by the CU-CP routines.
class cu_cp_ntn_tai_info_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_ntn_tai_info_test() : cu_cp_test_environment(cu_cp_test_env_params{}) { run_ng_setup(); }

  /// TACs the test cell broadcasts in trackingAreaList, TS 38.331. The first is the cell's primary TAC.
  static constexpr std::array<tac_t, 3> broadcast_tacs = {7, 8, 9};

  void attach_ue_and_get_reported_location(span<const tac_t>                    tac_list,
                                           asn1::ngap::user_location_info_nr_s& user_loc_info);
};

/// Connects a DU whose cell broadcasts \c tac_list, attaches a UE, and returns the User Location Information the
/// CU-CP reports to the AMF in the resulting Initial UE Message.
void cu_cp_ntn_tai_info_test::attach_ue_and_get_reported_location(span<const tac_t>                    tac_list,
                                                                  asn1::ngap::user_location_info_nr_s& user_loc_info)
{
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = ret.value();

  // F1AP carries at most one TAC per broadcast PLMN, so the CU-CP recovers the broadcast list from the DU's SIB1.
  ASSERT_TRUE(this->run_f1_setup(
      du_idx,
      int_to_gnb_du_id(0x11),
      {test_helpers::served_cell_item_info{
          .tac = 7, .sib1_str = test_helpers::create_sib1_hex_string(plmn_identity::test_value(), tac_list)}}));

  // The CU-CP rejects RRC connections until a CU-UP is available.
  ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  ASSERT_TRUE(this->run_e1_setup(ret.value()));

  // Attach a UE. Done inline rather than through connect_new_ue() so that the Initial UE Message stays available for
  // inspection instead of being consumed by the helper.
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_init_ul_rrc_message_transfer(du_ue_id, to_rnti(0x4601), plmn_identity::test_value()));

  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));

  get_du(du_idx).push_rrc_ul_dcch_message(
      du_ue_id, srb_id_t::srb1, test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete()));

  ngap_message ngap_pdu;
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(test_helpers::is_valid_init_ue_message(ngap_pdu));

  const auto& init_ue_msg = ngap_pdu.pdu.init_msg().value.init_ue_msg();
  ASSERT_EQ(init_ue_msg->user_location_info.type(),
            asn1::ngap::user_location_info_c::types_opts::options::user_location_info_nr);
  user_loc_info = init_ue_msg->user_location_info.user_location_info_nr();
}

TEST_F(cu_cp_ntn_tai_info_test, when_cell_broadcasts_several_tacs_then_initial_ue_message_lists_them_all)
{
  asn1::ngap::user_location_info_nr_s user_loc_info;
  attach_ue_and_get_reported_location(broadcast_tacs, user_loc_info);

  ASSERT_TRUE(user_loc_info.ie_exts.nr_ntn_tai_info_present)
      << "No NR NTN TAI Information reported, so the AMF is never told which tracking areas the cell serves";

  const auto& tac_list = user_loc_info.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn;
  ASSERT_EQ(tac_list.size(), broadcast_tacs.size());
  for (unsigned i = 0; i != broadcast_tacs.size(); ++i) {
    EXPECT_EQ(tac_list[i].to_number(), broadcast_tacs[i]) << "TAC List in NR NTN entry " << i;
  }

  // The TAI keeps carrying the F1AP TAC, so an AMF without NR NTN TAI Information still finds a tracking area.
  EXPECT_EQ(user_loc_info.tai.tac.to_number(), broadcast_tacs[0]);
}

TEST_F(cu_cp_ntn_tai_info_test,
       when_broadcast_tac_list_does_not_lead_with_the_5gs_tac_then_it_is_reported_in_broadcast_order)
{
  // TS 38.331 does not order trackingAreaList, so a gNB-DU may broadcast the F1AP 5GS TAC anywhere in the list.
  static constexpr std::array<tac_t, 3> unordered_tacs = {8, 7, 9};

  asn1::ngap::user_location_info_nr_s user_loc_info;
  attach_ue_and_get_reported_location(unordered_tacs, user_loc_info);

  ASSERT_TRUE(user_loc_info.ie_exts.nr_ntn_tai_info_present)
      << "A broadcast TAC list holding the 5GS TAC must be reported";

  // TS 38.413 reports the TACs the cell broadcasts, so the list keeps the broadcast order.
  const auto& tac_list = user_loc_info.ie_exts.nr_ntn_tai_info.tac_list_in_nr_ntn;
  ASSERT_EQ(tac_list.size(), unordered_tacs.size());
  for (unsigned i = 0, e = unordered_tacs.size(); i != e; ++i) {
    EXPECT_EQ(tac_list[i].to_number(), unordered_tacs[i]) << "TAC List in NR NTN entry " << i;
  }
  EXPECT_EQ(user_loc_info.tai.tac.to_number(), 7);
}

TEST_F(cu_cp_ntn_tai_info_test, when_broadcast_tac_list_lacks_the_5gs_tac_then_no_ntn_tai_information_is_reported)
{
  // A list without the F1AP 5GS TAC would disagree with the TAI the CU-CP reports.
  static constexpr std::array<tac_t, 2> foreign_tacs = {8, 9};

  asn1::ngap::user_location_info_nr_s user_loc_info;
  attach_ue_and_get_reported_location(foreign_tacs, user_loc_info);

  EXPECT_FALSE(user_loc_info.ie_exts.nr_ntn_tai_info_present);
  EXPECT_EQ(user_loc_info.tai.tac.to_number(), 7);
}

TEST_F(cu_cp_ntn_tai_info_test, when_cell_broadcasts_a_single_tac_then_no_ntn_tai_information_is_reported)
{
  asn1::ngap::user_location_info_nr_s user_loc_info;
  attach_ue_and_get_reported_location({}, user_loc_info);

  EXPECT_FALSE(user_loc_info.ie_exts.nr_ntn_tai_info_present) << "A TN cell must not report NR NTN TAI Information";
}
