// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::f1ap;

const std::chrono::milliseconds warning_procedure_timeout{100};

class f1ap_cu_write_replace_warning_test : public f1ap_cu_test
{
protected:
  f1ap_cu_write_replace_warning_test() : f1ap_cu_test(f1ap_configuration{.proc_timeout = warning_procedure_timeout}) {}

  f1ap_write_replace_warning_request make_request()
  {
    f1ap_write_replace_warning_request req;
    req.pws_sys_info.sib_type    = 6;
    req.pws_sys_info.sib_msg     = byte_buffer::create({0xde, 0xad, 0xbe, 0xef}).value();
    req.repeat_period            = 4;
    req.nof_broadcasts_requested = 2;
    return req;
  }

  void start_procedure(const f1ap_write_replace_warning_request& req)
  {
    t = f1ap->handle_write_replace_warning_request(req);
    t_launcher.emplace(t);
  }

  bool was_request_sent() const
  {
    if (f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != f1ap_pdu_c::types::init_msg) {
      return false;
    }
    return f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value ==
           f1ap_elem_procs_o::init_msg_c::types_opts::write_replace_warning_request;
  }

  async_task<f1ap_write_replace_warning_response>                        t;
  std::optional<lazy_task_launcher<f1ap_write_replace_warning_response>> t_launcher;
};

TEST_F(f1ap_cu_write_replace_warning_test, when_request_started_then_pdu_is_sent_and_procedure_waits)
{
  start_procedure(make_request());

  ASSERT_TRUE(was_request_sent());
  ASSERT_FALSE(t.ready());
}

TEST_F(f1ap_cu_write_replace_warning_test, when_response_received_then_procedure_succeeds)
{
  start_procedure(make_request());

  const auto&  req_pdu  = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.write_replace_warning_request();
  f1ap_message response = test_helpers::generate_f1ap_write_replace_warning_response(req_pdu->transaction_id);
  f1ap->handle_message(response);

  ASSERT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
}

TEST_F(f1ap_cu_write_replace_warning_test, when_response_contains_completed_cells_they_are_reported)
{
  start_procedure(make_request());

  const auto& req_pdu = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.write_replace_warning_request();
  const std::vector<nr_cell_global_id_t> completed{
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()}};
  f1ap_message response =
      test_helpers::generate_f1ap_write_replace_warning_response(req_pdu->transaction_id, completed);
  f1ap->handle_message(response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
  ASSERT_EQ(t.get().cells_broadcast_completed.size(), 1U);
  EXPECT_EQ(t.get().cells_broadcast_completed[0].plmn_id, plmn_identity::test_value());
}

TEST_F(f1ap_cu_write_replace_warning_test, when_timeout_reached_then_procedure_fails)
{
  start_procedure(make_request());

  for (unsigned i = 0; i != warning_procedure_timeout.count(); ++i) {
    ASSERT_FALSE(t.ready());
    tick();
  }

  ASSERT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
}
