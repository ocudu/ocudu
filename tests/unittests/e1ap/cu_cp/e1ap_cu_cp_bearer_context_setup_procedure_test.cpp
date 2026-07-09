// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_cp_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::e1ap;

class e1ap_cu_cp_bearer_context_setup_test : public e1ap_cu_cp_test
{
protected:
  void start_procedure(const e1ap_bearer_context_setup_request& req)
  {
    t = e1ap->handle_bearer_context_setup_request(req);
    t_launcher.emplace(t);

    ASSERT_FALSE(t.ready());
    ASSERT_EQ(this->e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
              e1ap_elem_procs_o::init_msg_c::types::bearer_context_setup_request);

    test_ues.emplace(req.ue_index, test_ue{req.ue_index});
    test_ue& u         = test_ues[req.ue_index];
    u.ue_index         = req.ue_index;
    u.cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(this->e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg()
                                                         .value.bearer_context_setup_request()
                                                         ->gnb_cu_cp_ue_e1ap_id);
  }

  bool was_bearer_context_setup_request_sent(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const
  {
    if (this->e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value != e1ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value !=
        e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request) {
      return false;
    }
    auto& req = this->e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.bearer_context_setup_request();

    return req->gnb_cu_cp_ue_e1ap_id == gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  }

  bool was_bearer_context_setup_successful() const
  {
    if (not t.ready()) {
      return false;
    }
    return t.get().success;
  }

  async_task<e1ap_bearer_context_setup_response>                        t;
  std::optional<lazy_task_launcher<e1ap_bearer_context_setup_response>> t_launcher;
};

TEST_F(e1ap_cu_cp_bearer_context_setup_test, when_request_sent_then_procedure_waits_for_response)
{
  // Test Preamble.
  auto request = generate_bearer_context_setup_request(uint_to_ue_index(test_rng::uniform_int<uint64_t>(
      cu_cp_ue_index_to_uint(cu_cp_ue_index_t::min), cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max))));

  // Start BEARER CONTEXT SETUP procedure.
  this->start_procedure(request);

  // The BEARER CONTEXT SETUP was sent to CU-UP and CU-CP is waiting for response.
  ASSERT_TRUE(was_bearer_context_setup_request_sent(test_ues[request.ue_index].cu_cp_ue_e1ap_id.value()));
  ASSERT_FALSE(t.ready());
}

TEST_F(e1ap_cu_cp_bearer_context_setup_test, when_response_received_then_procedure_successful)
{
  // Test Preamble.
  auto request = generate_bearer_context_setup_request(uint_to_ue_index(test_rng::uniform_int<uint64_t>(
      cu_cp_ue_index_to_uint(cu_cp_ue_index_t::min), cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max))));

  // Start BEARER CONTEXT SETUP procedure and return back the response from the CU-UP.
  this->start_procedure(request);

  auto& ue            = test_ues[request.ue_index];
  ue.cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(
      test_rng::uniform_int<uint64_t>(gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::min),
                                      gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::max)));
  e1ap_message response =
      generate_bearer_context_setup_response(ue.cu_cp_ue_e1ap_id.value(), ue.cu_up_ue_e1ap_id.value());
  e1ap->handle_message(response);

  // The BEARER CONTEXT SETUP RESPONSE was received and the CU-CP completed the procedure.
  ASSERT_TRUE(was_bearer_context_setup_successful());
}

TEST_F(e1ap_cu_cp_bearer_context_setup_test, when_failure_received_then_procedure_unsuccessful)
{
  // Test Preamble.
  auto request = generate_bearer_context_setup_request(uint_to_ue_index(test_rng::uniform_int<uint64_t>(
      cu_cp_ue_index_to_uint(cu_cp_ue_index_t::min), cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max))));

  // Start BEARER CONTEXT SETUP procedure and return back the failure response from the CU-UP.
  this->start_procedure(request);

  auto& ue            = test_ues[request.ue_index];
  ue.cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(
      test_rng::uniform_int<uint64_t>(gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::min),
                                      gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::max)));

  e1ap_message response =
      generate_bearer_context_setup_failure(ue.cu_cp_ue_e1ap_id.value(), ue.cu_up_ue_e1ap_id.value());
  e1ap->handle_message(response);

  // The BEARER CONTEXT SETUP FAILURE was received and the CU-CP completed the procedure with failure.
  ASSERT_FALSE(was_bearer_context_setup_successful());
}

TEST_F(e1ap_cu_cp_bearer_context_setup_test,
       when_e1ap_stopped_while_bearer_setup_in_flight_then_teardown_does_not_crash)
{
  // Test Preamble.
  auto request = generate_bearer_context_setup_request(uint_to_ue_index(test_rng::uniform_int<uint64_t>(
      cu_cp_ue_index_to_uint(cu_cp_ue_index_t::min), cu_cp_ue_index_to_uint(cu_cp_ue_index_t::max))));

  // Start BEARER CONTEXT SETUP procedure, leaving its transaction unanswered.
  this->start_procedure(request);

  // Stop E1AP while the Bearer Context Setup transaction is still in flight.
  async_task<void>         stop_task = e1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);

  ASSERT_TRUE(stop_task.ready());
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(was_bearer_context_setup_successful());
}
