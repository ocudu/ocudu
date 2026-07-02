// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_cp_test_helpers.h"
#include "ocudu/ran/cause/e1ap_cause.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::e1ap;

class e1ap_cu_cp_reset_test : public e1ap_cu_cp_test
{
protected:
  cu_cp_reset make_request()
  {
    cu_cp_reset reset;
    reset.cause           = e1ap_cause_radio_network_t::unspecified;
    reset.interface_reset = true;
    return reset;
  }

  void start_procedure(const cu_cp_reset& reset)
  {
    t = e1ap->handle_cu_cp_e1_reset_message(reset);
    t_launcher.emplace(t);
  }

  bool was_reset_sent() const
  {
    if (e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value != e1ap_pdu_c::types::init_msg) {
      return false;
    }
    return e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value ==
           e1ap_elem_procs_o::init_msg_c::types_opts::reset;
  }

  async_task<void>                        t;
  std::optional<lazy_task_launcher<void>> t_launcher;
};

/// If the E1AP is stopped (e.g. CU-UP removal) before an E1 Reset procedure starts, the request must not be sent.
/// Note: e1ap_cu_cp_impl::handle_cu_cp_e1_reset_message drops the request via its own "e1ap_stopping" flag before
/// cu_cp_e1_reset_procedure is even constructed, so this test exercises that outer guard rather than the
/// procedure's own transaction.aborted() guard (which is unreachable from this public API for that reason, but is
/// kept as defense-in-depth, e.g. for callers that don't go through handle_cu_cp_e1_reset_message).
TEST_F(e1ap_cu_cp_reset_test, when_e1ap_already_stopped_then_reset_is_not_sent)
{
  async_task<void>         stop_task = e1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);
  ASSERT_TRUE(stop_task.ready());

  start_procedure(make_request());

  ASSERT_FALSE(was_reset_sent());
  ASSERT_TRUE(t.ready());
}
