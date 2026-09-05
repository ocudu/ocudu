// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class ngap_nas_message_routine_test : public ngap_test
{
protected:
  cu_cp_ue_index_t start_procedure(rnti_t rnti = rnti_t::MIN_CRNTI) { return create_ue(rnti); }

  cu_cp_ue_index_t start_dl_nas_procedure()
  {
    cu_cp_ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF.
    run_dl_nas_transport(ue_index);

    return ue_index;
  }

  bool was_dl_nas_transport_forwarded(const test_ue& ue) const
  {
    return ue.rrc_ue_handler.last_nas_pdu.length() == nas_pdu_len;
  }

  bool was_dl_nas_transport_dropped(const test_ue& ue) const { return ue.rrc_ue_handler.last_nas_pdu.empty(); }

  bool was_ue_radio_capability_info_indication_sent() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_radio_cap_info_ind;
  }

  bool was_ul_nas_transport_forwarded() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ul_nas_transport;
  }

  bool was_ue_release_requested(const test_ue& ue) const { return cu_cp_notifier.last_command.ue_index == ue.ue_index; }

  bool was_error_indication_sent() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Initial UE message tests.
TEST_F(ngap_nas_message_routine_test, when_initial_ue_message_is_received_then_ngap_ue_is_created)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble.
  this->start_procedure();

  // Check that initial UE message is sent to AMF and that UE objects has been created.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);
}

/// Initial UE message tests.
TEST_F(ngap_nas_message_routine_test, when_initial_context_setup_request_is_not_received_then_ue_is_released)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble.
  this->start_procedure();

  // Check that initial UE message is sent to AMF and that UE objects has been created.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // Tick timers.
  // Status: NGAP does not receive new Initial Context Setup Request until request_pdu_session_timer has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < cu_cp_cfg.ue.request_pdu_session_timeout.count() * 1000;
       ++msec_elapsed) {
    this->tick();
  }

  // check that UE release was requested.
  ASSERT_NE(cu_cp_notifier.last_command.ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_EQ(cu_cp_notifier.last_command.cause, ngap_cause_t{ngap_cause_radio_network_t::unspecified});
}

/// Test DL NAS transport handling.
TEST_F(ngap_nas_message_routine_test, when_ue_present_dl_nas_transport_is_forwarded)
{
  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rng::uniform_int<uint64_t>(to_underlying(amf_ue_id_t::min), to_underlying(amf_ue_id_t::max)));

  // Inject DL NAS transport message from AMF.
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called.
  ASSERT_TRUE(was_dl_nas_transport_forwarded(ue));
}

TEST_F(ngap_nas_message_routine_test, when_no_ue_present_dl_nas_transport_is_dropped_and_error_indication_is_sent)
{
  // Inject DL NAS transport message from AMF.
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(
      uint_to_amf_ue_id(
          test_rng::uniform_int<uint64_t>(to_underlying(amf_ue_id_t::min), to_underlying(amf_ue_id_t::max))),
      uint_to_ran_ue_id(
          test_rng::uniform_int<uint64_t>(to_underlying(ran_ue_id_t::min), to_underlying(ran_ue_id_t::max))));
  ngap->handle_message(dl_nas_transport);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test DL NAS transport handling
TEST_F(ngap_nas_message_routine_test,
       when_dl_nas_transport_contains_ue_cap_info_request_then_ue_radio_cap_info_indication_is_sent)
{
  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rng::uniform_int<uint64_t>(to_underlying(amf_ue_id_t::min), to_underlying(amf_ue_id_t::max)));

  // Inject DL NAS transport message from AMF.
  ngap_message dl_nas_transport =
      generate_downlink_nas_transport_message_with_ue_cap_info_request(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called.
  ASSERT_TRUE(was_dl_nas_transport_forwarded(ue));

  // Check that UE Radio Capability Info Indication was sent to AMF.
  ASSERT_TRUE(was_ue_radio_capability_info_indication_sent());
}

/// Test DL NAS transport after transfering UE IDs/context.
TEST_F(ngap_nas_message_routine_test, when_ue_context_is_tranfered_amf_ue_id_is_updated)
{
  // Test preamble to get UE created.
  cu_cp_ue_index_t ue_index = this->start_procedure();
  // Inject DL NAS transport message from AMF.
  run_dl_nas_transport(ue_index);
  // Inject UL NAS transport message from RRC.
  run_ul_nas_transport(ue_index);

  auto& ue = test_ues.at(ue_index);
  ASSERT_NE(ue.ue_index, cu_cp_ue_index_t::invalid);

  // Get AMF UE ID
  amf_ue_id_t amf_id = ue.amf_ue_id.value();
  ASSERT_NE(amf_id, amf_ue_id_t::invalid);

  ran_ue_id_t ran_id = ue.ran_ue_id.value();
  ASSERT_NE(ran_id, ran_ue_id_t::invalid);

  // Clear NAS PDU.
  ue.rrc_ue_handler.last_nas_pdu.clear();
  ASSERT_TRUE(ue.rrc_ue_handler.last_nas_pdu.empty());

  // Inject new DL NAS transport from core.
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(amf_id, ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check NAS PDU has been passed to RRC.
  ASSERT_FALSE(ue.rrc_ue_handler.last_nas_pdu.empty());

  // Clear PDU again.
  ue.rrc_ue_handler.last_nas_pdu.clear();

  // Create new UE object (with own RRC UE notifier).
  cu_cp_ue_index_t target_ue_index = create_ue_without_init_ue_message(rnti_t::MAX_CRNTI);
  ASSERT_NE(target_ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_NE(target_ue_index, ue_index);
  auto& target_ue = test_ues.at(target_ue_index);
  ASSERT_TRUE(target_ue.rrc_ue_handler.last_nas_pdu.empty());

  // Transfer NGAP UE context to new target UE.
  ngap->update_ue_index(target_ue_index, ue_index, ue_mng.find_ue(target_ue_index)->get_ngap_cu_cp_ue_notifier());

  // Inject NAS message again.
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier of initial UE has not been called.
  ASSERT_TRUE(ue.rrc_ue_handler.last_nas_pdu.empty());

  // Verify that RRC notifier of target UE has indeed benn called.
  ASSERT_FALSE(target_ue.rrc_ue_handler.last_nas_pdu.empty());
}

/// Test UL NAS transport handling.
TEST_F(ngap_nas_message_routine_test, when_ue_present_and_amf_set_ul_nas_transport_is_forwared)
{
  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_dl_nas_procedure();

  cu_cp_ul_nas_transport ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);

  // Check that AMF notifier was called with right type.
  ASSERT_TRUE(was_ul_nas_transport_forwarded());
}

/// Test AMF UE ID handling.
TEST_F(ngap_nas_message_routine_test, when_amf_ue_id_is_max_size_then_its_not_cropped)
{
  // Test preamble
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = amf_ue_id_t::max;

  // Inject DL NAS transport message from AMF.
  ngap_message dl_nas_transport = generate_downlink_nas_transport_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called.
  ASSERT_TRUE(was_dl_nas_transport_forwarded(ue));

  cu_cp_ul_nas_transport ul_nas_transport = generate_ul_nas_transport_message(ue_index);
  ngap->handle_ul_nas_transport_message(ul_nas_transport);

  // Check that AMF notifier was called with right type.
  ASSERT_TRUE(was_ul_nas_transport_forwarded());

  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.ul_nas_transport()->amf_ue_ngap_id,
            to_underlying(amf_ue_id_t::max));
}

/// Test UE ID pair consistency.
TEST_F(ngap_nas_message_routine_test,
       when_amf_ue_id_pair_is_incosistent_then_old_ue_is_released_and_error_indication_is_sent)
{
  // Test preamble.
  cu_cp_ue_index_t ue_index1 = this->start_procedure();
  cu_cp_ue_index_t ue_index2 = this->start_procedure(to_rnti(0x2));

  auto&    ue1 = test_ues.at(ue_index1);
  auto&    ue2 = test_ues.at(ue_index2);
  unsigned amf_id{};

  // Inject DL NAS transport message from AMF on UE 1.
  ngap_message dl_nas_transport =
      generate_downlink_nas_transport_message(uint_to_amf_ue_id(amf_id), ue1.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Check that RRC notifier was called.
  ASSERT_TRUE(was_dl_nas_transport_forwarded(ue1));

  // Inject DL NAS transport message with valid RAN UE ID, but wrong AMF UE ID.
  ngap_message inconsistent_dl_nas_transport =
      generate_downlink_nas_transport_message(uint_to_amf_ue_id(amf_id), ue2.ran_ue_id.value());
  ngap->handle_message(inconsistent_dl_nas_transport);

  // Check that RRC notifier was called.
  ASSERT_FALSE(was_dl_nas_transport_forwarded(ue2));

  // Check that release of old UE has been requested.
  ASSERT_TRUE(was_ue_release_requested(ue1));

  // Check that error indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.error_ind()->cause.radio_network(),
            asn1::ngap::cause_radio_network_e::options::inconsistent_remote_ue_ngap_id);
}
