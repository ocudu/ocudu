// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rrc_ue_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class RRC Reestablishment tests preparation
class rrc_ue_reest : public rrc_ue_test_helper, public ::testing::Test
{
protected:
  static void SetUpTestSuite() { ocudulog::init(); }

  void SetUp() override { init(); }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest, when_invalid_reestablishment_request_received_then_rrc_setup_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  receive_invalid_reestablishment_request(0, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest, when_valid_reestablishment_request_received_but_security_context_not_found_then_rrc_setup_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest, when_reestablishment_request_with_cause_recfg_fail_received_then_rrc_setup_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  cu_cp_ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request_with_cause_recfg_fail(1, to_rnti(0x4601));

  // check if the RRC Setup Request was generated
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();

  check_initial_ue_message_sent();
}

/// Test the RRC Reestablishment
TEST_F(rrc_ue_reest,
       when_valid_reestablishment_request_for_same_du_received_then_rrc_reestablishment_with_old_ue_index_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  cu_cp_ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_validation_requests, 1U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_context_transfers, 1U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_setup_requests, 1U);

  // check if SRB1 was created
  check_srb1_exists();

  // check if the RRC message was sent over SRB1
  ASSERT_EQ(get_last_srb(), srb_id_t::srb1);

  receive_reestablishment_complete();
  ASSERT_EQ(rrc_ue->get_rrc_state(), rrc_state::connected);
}

TEST_F(rrc_ue_reest, when_ue_answers_rrc_reestablishment_with_another_message_then_ue_is_not_connected)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  static constexpr uint8_t reest_transaction_id = 0;

  cu_cp_ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  receive_valid_reestablishment_request(1, to_rnti(0x4601));
  ASSERT_EQ(get_last_srb(), srb_id_t::srb1);

  // Receive RRC Reconfiguration Complete with the transaction ID of the RRC Reestablishment
  receive_ul_dcch_msg(test_helpers::create_rrc_reconfiguration_complete(reest_transaction_id));

  ASSERT_NE(rrc_ue->get_rrc_state(), rrc_state::connected);
}

TEST_F(rrc_ue_reest, when_no_local_context_matches_then_context_is_retrieved_from_peer)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  // No local UE context is added, so the context can only come from a peer NG-RAN node (TS 38.423 section 8.2.4).
  add_retrievable_ue_context();

  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  // The retrieval must carry the identity the UE used, so the peer can verify the ShortMAC-I against it.
  ASSERT_TRUE(rrc_ue_cu_cp_notifier.last_context_retrieval_request.has_value())
      << "No UE context retrieval was requested";
  const auto& retrieval_id = rrc_ue_cu_cp_notifier.last_context_retrieval_request->ue_id;
  ASSERT_TRUE(std::holds_alternative<rrc_ue_context_retrieval_id_for_reest>(retrieval_id));
  ASSERT_EQ(std::get<rrc_ue_context_retrieval_id_for_reest>(retrieval_id).old_pci, 1);
  ASSERT_EQ(std::get<rrc_ue_context_retrieval_id_for_reest>(retrieval_id).old_c_rnti, to_rnti(0x4601));

  // The wait for the peer is spent out of the UE's running T301, so it must leave room for the RRC Setup fallback.
  const std::chrono::milliseconds max_response_time =
      rrc_ue_cu_cp_notifier.last_context_retrieval_request->max_response_time;
  ASSERT_GT(max_response_time.count(), 0) << "The retrieval would time out immediately";
  ASSERT_LT(max_response_time, test_t301) << "The retrieval may consume the UE's entire T301";

  // The UE is answered with RRCReestablishment over SRB1, not with an RRC Setup fallback.
  check_srb1_exists();
  ASSERT_EQ(get_last_srb(), srb_id_t::srb1);

  receive_reestablishment_complete();
}

TEST_F(rrc_ue_reest, when_context_cannot_be_retrieved_from_peer_then_rrc_setup_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  // No local context and no peer that can supply one, e.g. no Xn-C peer serves the failure cell.
  receive_valid_reestablishment_request(1, to_rnti(0x4601));

  ASSERT_TRUE(rrc_ue_cu_cp_notifier.last_context_retrieval_request.has_value())
      << "No UE context retrieval was requested";

  // Fall back to RRC Setup, as before.
  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  check_srb1_exists();
  receive_setup_complete();
  check_initial_ue_message_sent();
}

TEST_F(rrc_ue_reest, when_reestablishment_is_rejected_locally_then_no_context_is_retrieved_from_peer)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  // A local context exists but cannot be reestablished from. Asking a peer for it would be pointless, and would delay
  // the RRC Setup fallback while the UE's T301 runs.
  cu_cp_ue_index_t old_ue_index = uint_to_ue_index(0);
  add_ue_reestablishment_context(old_ue_index);
  add_retrievable_ue_context();

  receive_valid_reestablishment_request_with_cause_recfg_fail(1, to_rnti(0x4601));

  ASSERT_FALSE(rrc_ue_cu_cp_notifier.last_context_retrieval_request.has_value())
      << "A UE context retrieval was requested for a locally rejected reestablishment";

  ASSERT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);
}

TEST_F(rrc_ue_reest, incompatible_imported_up_context_falls_back_to_setup)
{
  add_ue_reestablishment_context(uint_to_ue_index(0));
  rrc_ue_cu_cp_notifier.up_context_validation_succeeds = false;
  receive_valid_reestablishment_request(1, to_rnti(0x4601));
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_validation_requests, 1U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_context_transfers, 0U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_setup_requests, 0U);
  EXPECT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);
}

TEST_F(rrc_ue_reest, malformed_imported_capabilities_do_not_import_up_context)
{
  rrc_ue_cap_rat_container_list_t caps;
  caps.push_back({rat_type_t::nr, byte_buffer::create({0xff}).value()});
  add_ue_reestablishment_context(uint_to_ue_index(0), std::move(caps));
  receive_valid_reestablishment_request(1, to_rnti(0x4601));
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_setup_requests, 0U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_context_transfers, 0U);
  EXPECT_EQ(get_srb0_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);
}

TEST_F(rrc_ue_reest, when_validated_context_import_fails_after_transfer_then_ue_is_released)
{
  add_ue_reestablishment_context(uint_to_ue_index(0));
  rrc_ue_cu_cp_notifier.up_context_setup_succeeds = false;
  receive_valid_reestablishment_request(1, to_rnti(0x4601));
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_validation_requests, 1U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_ue_context_transfers, 1U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.nof_up_context_setup_requests, 1U);
  EXPECT_EQ(rrc_ue_cu_cp_notifier.last_cu_cp_ue_context_release_request.ue_index, allocated_ue_index);
  EXPECT_TRUE(rrc_ue_f1ap_notifier.last_rrc_pdu.empty());
}
