// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::f1ap;

const std::chrono::milliseconds procedure_timeout{100};

class f1ap_cu_ue_context_setup_test : public f1ap_cu_test
{
protected:
  f1ap_cu_ue_context_setup_test() : f1ap_cu_test(f1ap_configuration{.proc_timeout = procedure_timeout}) {}

  void start_procedure(const f1ap_ue_context_setup_request& req)
  {
    t = f1ap->handle_ue_context_setup_request(req, {});
    t_launcher.emplace(t);

    EXPECT_EQ(this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value,
              f1ap_elem_procs_o::init_msg_c::types::ue_context_setup_request);
  }

  bool was_ue_context_setup_request_sent() const
  {
    if (this->f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != f1ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value !=
        asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request) {
      return false;
    }
    return true;
  }

  bool was_ue_context_setup_response_received() const
  {
    if (not t.ready()) {
      return false;
    }
    return t.get().success;
  }

  bool was_ue_context_setup_failure_received() const
  {
    if (not t.ready()) {
      return false;
    }

    return !t.get().success;
  }

  async_task<f1ap_ue_context_setup_response>                        t;
  std::optional<lazy_task_launcher<f1ap_ue_context_setup_response>> t_launcher;
};

TEST_F(f1ap_cu_ue_context_setup_test, when_request_sent_then_procedure_waits_for_response)
{
  // Start UE CONTEXT SETUP procedure.
  this->start_procedure(create_ue_context_setup_request({drb_id_t::drb1}));

  // The UE CONTEXT SETUP was sent to DU and F1AP-CU is waiting for response.
  ASSERT_TRUE(was_ue_context_setup_request_sent());
  ASSERT_FALSE(t.ready());
}

TEST_F(f1ap_cu_ue_context_setup_test, when_response_received_then_procedure_successful)
{
  // Start UE CONTEXT SETUP procedure and return back the response from the DU.
  this->start_procedure(create_ue_context_setup_request({drb_id_t::drb1}));
  f1ap_message response = test_helpers::generate_ue_context_setup_response(
      int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(test_rng::uniform_int<uint32_t>()));
  f1ap->handle_message(response);

  // The UE CONTEXT SETUP RESPONSE was received and the F1AP-CU completed the procedure.
  ASSERT_TRUE(was_ue_context_setup_response_received());
}

TEST_F(f1ap_cu_ue_context_setup_test, when_ue_setup_failure_received_then_procedure_unsuccessful)
{
  // Start UE CONTEXT SETUP procedure and return back the failure response from the DU.
  this->start_procedure(create_ue_context_setup_request({drb_id_t::drb1}));

  f1ap_message response = test_helpers::generate_ue_context_setup_failure(
      int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(test_rng::uniform_int<uint32_t>()));
  f1ap->handle_message(response);

  // The UE CONTEXT SETUP FAILURE was received and the F1AP-CU completed the procedure with failure.
  ASSERT_TRUE(was_ue_context_setup_failure_received());

  // The F1AP-CU removed the UE context that the procedure created.
  ASSERT_EQ(f1ap->get_nof_ues(), 0);
}

TEST_F(f1ap_cu_ue_context_setup_test, when_ue_setup_procedure_timeouts_then_procedure_unsuccessful)
{
  // Start UE CONTEXT SETUP procedure and return back the failure response from the DU.
  this->start_procedure(create_ue_context_setup_request({drb_id_t::drb1}));

  for (unsigned i = 0; i != procedure_timeout.count(); ++i) {
    ASSERT_FALSE(was_ue_context_setup_failure_received());
    this->tick();
  }
  ASSERT_TRUE(was_ue_context_setup_failure_received());
}

TEST_F(f1ap_cu_ue_context_setup_test, when_f1ap_stopped_while_ue_context_setup_in_flight_then_teardown_does_not_crash)
{
  this->start_procedure(create_ue_context_setup_request({drb_id_t::drb1}));
  ASSERT_FALSE(t.ready());

  async_task<void>         stop_task = f1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);

  ASSERT_TRUE(stop_task.ready());
}

TEST_F(f1ap_cu_ue_context_setup_test,
       when_ue_setup_failure_received_for_existing_ue_then_ue_context_is_kept_and_can_be_released)
{
  // Create the UE with an INITIAL UL RRC MESSAGE TRANSFER.
  const test_ue& ue = create_ue(int_to_gnb_du_ue_f1ap_id(41255));

  // Start UE CONTEXT SETUP procedure for the existing UE and return back the failure response from the DU.
  f1ap_ue_context_setup_request req = create_ue_context_setup_request({drb_id_t::drb1});
  req.ue_index                      = ue.ue_index;
  this->start_procedure(req);

  f1ap_message response =
      test_helpers::generate_ue_context_setup_failure(int_to_gnb_cu_ue_f1ap_id(0), ue.du_ue_id.value());
  f1ap->handle_message(response);
  ASSERT_TRUE(was_ue_context_setup_failure_received());

  // The F1AP-CU keeps the UE context, because the DU also keeps it.
  ASSERT_EQ(f1ap->get_nof_ues(), 1);

  // The UE release sends the UE CONTEXT RELEASE COMMAND to the DU.
  f1ap_ue_context_release_command rel_cmd;
  rel_cmd.ue_index = ue.ue_index;
  rel_cmd.cause    = f1ap_cause_radio_network_t::unspecified;

  async_task<cu_cp_ue_index_t>         rel_task = f1ap->handle_ue_context_release_command(rel_cmd);
  lazy_task_launcher<cu_cp_ue_index_t> rel_launcher(rel_task);
  ASSERT_EQ(this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value,
            f1ap_elem_procs_o::init_msg_c::types::ue_context_release_cmd);
}

TEST_F(f1ap_cu_ue_context_setup_test, when_ue_setup_procedure_timeouts_for_existing_ue_then_ue_context_is_kept)
{
  // Create the UE with an INITIAL UL RRC MESSAGE TRANSFER.
  const test_ue& ue = create_ue(int_to_gnb_du_ue_f1ap_id(41255));

  // Start UE CONTEXT SETUP procedure for the existing UE and let it time out.
  f1ap_ue_context_setup_request req = create_ue_context_setup_request({drb_id_t::drb1});
  req.ue_index                      = ue.ue_index;
  this->start_procedure(req);

  for (unsigned i = 0; i != procedure_timeout.count(); ++i) {
    this->tick();
  }
  ASSERT_TRUE(was_ue_context_setup_failure_received());

  // The F1AP-CU keeps the UE context, because the DU can still have it.
  ASSERT_EQ(f1ap->get_nof_ues(), 1);
}
