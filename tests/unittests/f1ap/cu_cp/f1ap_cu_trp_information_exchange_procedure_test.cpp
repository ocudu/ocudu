// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::f1ap;

class f1ap_cu_trp_information_exchange_test : public f1ap_cu_test
{
protected:
  void start_procedure(const trp_information_request_t& req)
  {
    t = f1ap->handle_trp_information_request(req);
    t_launcher.emplace(t);
  }

  bool was_request_sent() const
  {
    if (f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != f1ap_pdu_c::types::init_msg) {
      return false;
    }
    return f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value ==
           f1ap_elem_procs_o::init_msg_c::types_opts::trp_info_request;
  }

  async_task<expected<trp_information_response_t, trp_information_failure_t>>                        t;
  std::optional<lazy_task_launcher<expected<trp_information_response_t, trp_information_failure_t>>> t_launcher;
};

/// If the F1AP is stopped (e.g. DU removal) before a TRP Information Exchange procedure starts, the request must
/// not be sent, since its transaction is cancelled before the procedure even gets to send it.
TEST_F(f1ap_cu_trp_information_exchange_test, when_f1ap_already_stopped_then_request_is_not_sent)
{
  async_task<void>         stop_task = f1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);
  ASSERT_TRUE(stop_task.ready());

  start_procedure(trp_information_request_t{});

  ASSERT_FALSE(was_request_sent());
  ASSERT_TRUE(t.ready());
  EXPECT_FALSE(t.get().has_value());
}
