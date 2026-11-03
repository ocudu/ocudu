// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/asn1/asn1_dispatch.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/async/coroutine.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {
asn1::e2ap::ric_sub_request_s make_subscription_request(uint16_t ran_function_id = 1)
{
  asn1::e2ap::ric_sub_request_s request;
  request->ran_function_id                 = ran_function_id;
  request->ric_request_id.ric_requestor_id = 123;
  request->ric_request_id.ric_instance_id  = 1;
  request->ric_sub_details.ric_action_to_be_setup_list.resize(1);
  request->ric_sub_details.ric_action_to_be_setup_list[0].value().ric_action_to_be_setup_item().ric_action_type =
      asn1::e2ap::ric_action_type_e::report;
  return request;
}
} // namespace

TEST_F(e2_test_subscriber, starting_unknown_subscription_does_not_create_subscription)
{
  auto             request = make_subscription_request();
  e2_event_manager events(factory);
  EXPECT_FALSE(e2_subscription_mngr->start_subscription(request->ric_request_id, 1, events, *msg_notifier, factory));
  EXPECT_TRUE(events.sub_del_reqs.empty());
  EXPECT_TRUE(e2_subscription_mngr->handle_subscription_setup(request).success);
  e2_subscription_mngr->stop();
}

TEST_F(e2_test_subscriber, starting_subscription_without_e2sm_removes_subscription)
{
  auto request = make_subscription_request();
  ASSERT_TRUE(e2_subscription_mngr->handle_subscription_setup(request).success);
  e2_event_manager events(factory);
  EXPECT_FALSE(
      e2_subscription_mngr->start_subscription(request->ric_request_id, 65535, events, *msg_notifier, factory));
  EXPECT_TRUE(events.sub_del_reqs.empty());
  EXPECT_TRUE(e2_subscription_mngr->handle_subscription_setup(request).success);
  e2_subscription_mngr->stop();
}

TEST_F(e2_test_subscriber, stopping_subscription_without_event_source_removes_subscription)
{
  auto                                 request = make_subscription_request();
  asn1::e2ap::ric_sub_delete_request_s deletion;
  deletion->ric_request_id  = request->ric_request_id;
  deletion->ran_function_id = request->ran_function_id;
  e2_event_manager events(factory);
  for (bool null_source : {false, true}) {
    ASSERT_TRUE(e2_subscription_mngr->handle_subscription_setup(request).success);
    if (null_source) {
      events.sub_del_reqs[{123, 1}] = nullptr;
    }
    e2_subscription_mngr->stop_subscription(request->ric_request_id, events, deletion);
    EXPECT_TRUE(events.sub_del_reqs.empty());
    EXPECT_FALSE(e2_subscription_mngr->handle_subscription_delete(deletion).success);
  }
}

TEST_F(e2_test_subscriber, unsupported_ran_function_returns_specific_failure)
{
  auto       request = make_subscription_request(65535);
  e2_message message;
  message.pdu.set_init_msg().load_info_obj(ASN1_E2AP_ID_RIC_SUB);
  message.pdu.init_msg().value.ric_sub_request() = request;
  e2->handle_message(message);
  task_worker.run_pending_tasks();
  const auto& pdu = e2_client->last_tx_e2_pdu.pdu;
  ASSERT_EQ(pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  const auto& failure = pdu.unsuccessful_outcome().value.ric_sub_fail();
  ASSERT_EQ(failure->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  EXPECT_EQ(failure->cause.ric_request(), asn1::e2ap::cause_ric_request_e::ran_function_id_invalid);
  EXPECT_EQ(failure->ran_function_id, 65535);
  EXPECT_EQ(failure->ric_request_id.ric_requestor_id, 123);
  EXPECT_EQ(failure->ric_request_id.ric_instance_id, 1);
}

TEST_F(e2_test_subscriber, undecodable_event_trigger_returns_specific_failure)
{
  static_cast<dummy_e2sm_handler&>(*e2sm_kpm_packer).event_trigger_definition.ric_service_type =
      e2sm_event_trigger_definition::UNKNOWN;
  auto       request = make_subscription_request();
  e2_message message;
  message.pdu.set_init_msg().load_info_obj(ASN1_E2AP_ID_RIC_SUB);
  message.pdu.init_msg().value.ric_sub_request() = request;
  e2->handle_message(message);
  task_worker.run_pending_tasks();
  const auto& pdu = e2_client->last_tx_e2_pdu.pdu;
  ASSERT_EQ(pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  const auto& failure = pdu.unsuccessful_outcome().value.ric_sub_fail();
  ASSERT_EQ(failure->cause.type().value, asn1::e2ap::cause_c::types_opts::ric_request);
  EXPECT_EQ(failure->cause.ric_request(), asn1::e2ap::cause_ric_request_e::event_trigger_not_supported);
  EXPECT_EQ(failure->ric_request_id.ric_requestor_id, 123);
  EXPECT_EQ(failure->ric_request_id.ric_instance_id, 1);

  // Rejected request leaves no subscription behind
  asn1::e2ap::ric_sub_delete_request_s deletion;
  deletion->ric_request_id = request->ric_request_id;
  EXPECT_FALSE(e2_subscription_mngr->handle_subscription_delete(deletion).success);
}

TEST_F(e2_test_subscriber, setup_failure_without_explicit_cause_uses_unspecified)
{
  auto request = make_subscription_request();
  request->ric_sub_details.ric_action_to_be_setup_list.resize(0);
  e2_message message;
  message.pdu.set_init_msg().load_info_obj(ASN1_E2AP_ID_RIC_SUB);
  message.pdu.init_msg().value.ric_sub_request() = request;
  e2->handle_message(message);
  task_worker.run_pending_tasks();
  const auto& pdu = e2_client->last_tx_e2_pdu.pdu;
  ASSERT_EQ(pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  const auto& cause = pdu.unsuccessful_outcome().value.ric_sub_fail()->cause;
  ASSERT_EQ(cause.type().value, asn1::e2ap::cause_c::types_opts::misc);
  EXPECT_EQ(cause.misc(), asn1::e2ap::cause_misc_e::unspecified);
  byte_buffer   bytes;
  asn1::bit_ref bits(bytes);
  EXPECT_EQ(asn1::pack(bits, pdu), asn1::OCUDUASN_SUCCESS);
}

TEST_F(e2_test_subscriber, invalid_decoded_report_period_is_normalized_before_indication_start)
{
  for (auto period : {std::pair<uint64_t, unsigned>{0, 1000}, {std::numeric_limits<uint64_t>::max(), 3600000}}) {
    SCOPED_TRACE(period.first);
    static_cast<dummy_e2sm_handler&>(*e2sm_kpm_packer).event_trigger_definition.report_period = period.first;
    auto                  request = make_subscription_request();
    e2_event_manager      events(factory);
    dummy_e2_pdu_notifier notifier(nullptr);
    ASSERT_TRUE(e2_subscription_mngr->handle_subscription_setup(request).success);
    ASSERT_TRUE(e2_subscription_mngr->start_subscription(request->ric_request_id, 1, events, notifier, factory));
    for (unsigned i = 0; i < period.second - 1; ++i) {
      tick();
    }
    EXPECT_EQ(notifier.last_e2_msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::nulltype);
    tick();
    EXPECT_EQ(notifier.last_e2_msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
    asn1::e2ap::ric_sub_delete_request_s deletion;
    deletion->ric_request_id = request->ric_request_id;
    e2_subscription_mngr->stop_subscription(request->ric_request_id, events, deletion);
  }
}
/// Test successful cu-cp initiated e2 setup procedure
TEST_F(e2_test_subscriber, when_e2_subscription_request_correct_sent_subscription_response)
{
  using namespace asn1::e2ap;
  // Action 1: Create valid e2 message
  uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                            0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                            0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                            0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer e2ap_sub_req_buf = byte_buffer::create(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req)).value();
  packer->handle_packed_pdu(std::move(e2ap_sub_req_buf));

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  e2_subscription_mngr->stop();
  e2->handle_e2_disconnection_request();
}
TEST_F(e2_test_subscriber, when_e2_subscription_request_received_start_indication_procedure)
{
  using namespace asn1::e2ap;
  // Action 1: Create valid e2 message
  uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                            0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                            0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                            0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer e2ap_sub_req_buf = byte_buffer::create(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req)).value();
  packer->handle_packed_pdu(std::move(e2ap_sub_req_buf));

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  for (int i = 0; i < 1500; i++) {
    this->tick();
  }
  e2_message& msg1 = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg1.pdu.init_msg().value.type(), e2ap_elem_procs_o::init_msg_c::types_opts::ric_ind);
  e2_subscription_mngr->stop();
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_subscriber, start_indication_procedure_check_contents)
{
  using namespace asn1::e2ap;
  // subscription info
  e2_subscription_info_t sub_info        = {};
  sub_info.report_period                 = std::chrono::milliseconds{1000};
  sub_info.request_id.ric_instance_id    = 1;
  sub_info.request_id.ric_requestor_id   = 2;
  ric_action_t                    action = {};
  asn1::unbounded_octstring<true> action_def;
  action_def.from_number(01020304);
  std::unique_ptr<e2sm_report_service> report_service =
      e2sm_mngr->get_e2sm_interface(e2sm_kpm_asn1_packer::oid)->get_e2sm_report_service(action_def);
  sub_info.action_list.push_back(
      {action_def.deep_copy().value(), 1, asn1::e2ap::ric_action_type_e::report, std::move(report_service)});
  std::unique_ptr<e2_event_manager> ev_mng = std::make_unique<e2_event_manager>(factory);
  ev_mng->add_sub_del_req(sub_info.request_id, factory);
  auto task = launch_async<e2ap_indication_procedure>(*msg_notifier, *ev_mng, sub_info, test_logger);

  ASSERT_FALSE(task.ready());
  for (int i = 0; i < 1500; i++) {
    this->tick();
  }

  asn1::cbit_ref                 bref1(msg_notifier->last_e2_msg.pdu.init_msg().value.ric_ind()->ric_ind_msg);
  asn1::e2sm::e2sm_kpm_ind_msg_s ind_msg1 = {};
  if (asn1::unpack(ind_msg1, bref1) != asn1::OCUDUASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(ind_msg1.ind_msg_formats.type(),
            asn1::e2sm::e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format2);

  ASSERT_EQ(ind_msg1.ind_msg_formats.ind_msg_format2().granul_period, 10);
  // If no conditions provided in the RIC Subscription Request, then we fill it with no_label
  for (auto& meas_item : ind_msg1.ind_msg_formats.ind_msg_format2().meas_cond_ue_id_list) {
    for (auto match_cond_it : meas_item.matching_cond) {
      ASSERT_EQ(match_cond_it.matching_cond_choice.meas_label().no_label_present, true);
    }
  }
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_subscriber, start_subscription_then_delete_subscription)
{
  using namespace asn1::e2ap;
  // Action 1: Create valid e2 message
  uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                            0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                            0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                            0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer e2ap_sub_req_buf = byte_buffer::create(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req)).value();
  packer->handle_packed_pdu(std::move(e2ap_sub_req_buf));

  e2_message& msg = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg.pdu.successful_outcome().value.type(),
            e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_sub_resp);

  uint8_t     sub_del_req[]   = {0x00, 0x09, 0x00, 0x12, 0x00, 0x00, 0x02, 0x00, 0x1d, 0x00, 0x05,
                                 0x00, 0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x93};
  byte_buffer sub_del_req_buf = byte_buffer::create(sub_del_req, sub_del_req + sizeof(sub_del_req)).value();

  packer->handle_packed_pdu(std::move(sub_del_req_buf));

  e2_message& msg1 = e2_client->last_tx_e2_pdu;
  ASSERT_EQ(msg1.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg1.pdu.successful_outcome().value.type(),
            e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_sub_delete_resp);
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_subscriber, when_start_subscription_called_twice_on_active_ric_request_id_then_second_call_is_ignored)
{
  using namespace asn1::e2ap;

  const uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                                  0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                                  0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                                  0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer sub_req = byte_buffer::create(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req)).value();
  packer->handle_packed_pdu(std::move(sub_req));

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, e2ap_pdu_c::types_opts::successful_outcome);

  for (int i = 0; i < 5; ++i) {
    tick();
  }

  const ric_sub_resp_s&              sub_resp = e2_client->last_tx_e2_pdu.pdu.successful_outcome().value.ric_sub_resp();
  const asn1::e2ap::ric_request_id_s request_id  = sub_resp->ric_request_id;
  const uint16_t                     ran_func_id = sub_resp->ran_function_id;

  // A second start_subscription() for an already-active RIC request ID must be rejected. The event manager and notifier
  // passed here are never used, since the call is rejected before they are accessed.
  e2_event_manager      duplicate_ev_mng(factory);
  dummy_e2_pdu_notifier duplicate_notifier(nullptr);
  ASSERT_FALSE(
      e2_subscription_mngr->start_subscription(request_id, ran_func_id, duplicate_ev_mng, duplicate_notifier, factory));

  for (int i = 0; i < 5; ++i) {
    tick();
  }

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type(), e2ap_elem_procs_o::init_msg_c::types_opts::ric_ind);

  e2_subscription_mngr->stop();
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_subscriber, when_duplicate_ric_subscription_request_reuses_active_request_id_then_rejects_without_crash)
{
  using namespace asn1::e2ap;

  const uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                                  0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                                  0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                                  0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer first_sub_req = byte_buffer::create(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req)).value();
  packer->handle_packed_pdu(std::move(first_sub_req));

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.successful_outcome().value.type(),
            e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_sub_resp);

  for (int i = 0; i < 30; ++i) {
    tick();
  }

  byte_buffer duplicate_sub_req = byte_buffer::create(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req)).value();
  packer->handle_packed_pdu(std::move(duplicate_sub_req));

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.unsuccessful_outcome().value.type(),
            e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_sub_fail);

  const auto& failure = e2_client->last_tx_e2_pdu.pdu.unsuccessful_outcome().value.ric_sub_fail();
  EXPECT_EQ(failure->cause.type().value, cause_c::types_opts::ric_request);
  if (failure->cause.type().value == cause_c::types_opts::ric_request) {
    EXPECT_EQ(failure->cause.ric_request(), cause_ric_request_e::duplicate_ric_request_id);
  }

  for (int i = 0; i < 30; ++i) {
    tick();
  }

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type(), e2ap_elem_procs_o::init_msg_c::types_opts::ric_ind);

  e2_subscription_mngr->stop();
  e2->handle_e2_disconnection_request();
}

TEST_F(e2_test_subscriber, subscription_report_period_controls_indication_timing)
{
  for (auto period : {std::pair<uint64_t, unsigned>{10, 10}, {3600000, 3600000}, {4294967295, 3600000}}) {
    SCOPED_TRACE(period.first);
    e2sm_kpm_asn1_packer kpm_packer(*du_meas_provider);
    e2sm_manager         services(test_logger);
    services.add_e2sm_service(e2sm_kpm_asn1_packer::oid,
                              std::make_unique<e2sm_kpm_impl>(test_logger, kpm_packer, *du_meas_provider));
    services.add_supported_ran_function(1, e2sm_kpm_asn1_packer::oid);
    e2_subscription_manager_impl manager(services);
    manager.add_ran_function_oid(1, e2sm_kpm_asn1_packer::oid);
    e2_event_manager      events(factory);
    dummy_e2_pdu_notifier notifier(nullptr);
    auto                  request = make_subscription_request();

    asn1::e2sm::e2sm_kpm_action_definition_s definition;
    definition.ric_style_type = 1;
    auto& format              = definition.action_definition_formats.set_action_definition_format1();
    format.granul_period      = 10;
    format.meas_info_list.resize(1);
    format.meas_info_list[0].meas_type.set_meas_name().from_string("DRB.UEThpUl");
    format.meas_info_list[0].label_info_list.resize(1);
    format.meas_info_list[0].label_info_list[0].meas_label.no_label_present = true;
    format.meas_info_list[0].label_info_list[0].meas_label.no_label =
        asn1::e2sm::meas_label_s::no_label_opts::true_value;
    request->ric_sub_details.ric_action_to_be_setup_list[0].value().ric_action_to_be_setup_item() =
        generate_e2sm_kpm_ric_action(definition);
    asn1::e2sm::e2sm_kpm_event_trigger_definition_s trigger;
    trigger.event_definition_formats.set_event_definition_format1().report_period = period.first;
    byte_buffer   trigger_bytes;
    asn1::bit_ref bits(trigger_bytes);
    ASSERT_EQ(asn1::pack(bits, trigger), asn1::OCUDUASN_SUCCESS);
    request->ric_sub_details.ric_event_trigger_definition = std::move(trigger_bytes);

    ASSERT_TRUE(manager.handle_subscription_setup(request).success);
    ASSERT_TRUE(manager.start_subscription(request->ric_request_id, 1, events, notifier, factory));
    for (unsigned i = 0; i < period.second - 1; ++i) {
      tick();
    }
    EXPECT_EQ(notifier.last_e2_msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::nulltype);
    tick();
    EXPECT_EQ(notifier.last_e2_msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
    asn1::e2ap::ric_sub_delete_request_s deletion;
    deletion->ric_request_id = request->ric_request_id;
    manager.stop_subscription(request->ric_request_id, events, deletion);
  }
}
