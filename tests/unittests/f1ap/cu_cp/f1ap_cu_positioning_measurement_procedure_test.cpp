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

class f1ap_cu_positioning_measurement_test : public f1ap_cu_test
{
protected:
  measurement_request_t make_request()
  {
    measurement_request_t req;
    req.lmf_meas_id            = lmf_meas_id_t::min;
    req.ran_meas_id            = ran_meas_id_t::min;
    req.report_characteristics = report_characteristics_t::on_demand;
    return req;
  }

  void start_procedure(const measurement_request_t& req)
  {
    t = f1ap->handle_positioning_measurement_request(req);
    t_launcher.emplace(t);
  }

  bool was_request_sent() const
  {
    if (f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != f1ap_pdu_c::types::init_msg) {
      return false;
    }
    return f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value ==
           f1ap_elem_procs_o::init_msg_c::types_opts::positioning_meas_request;
  }

  async_task<expected<measurement_response_t, measurement_failure_t>>                        t;
  std::optional<lazy_task_launcher<expected<measurement_response_t, measurement_failure_t>>> t_launcher;
};

/// If the F1AP is stopped (e.g. DU removal) before a Positioning Measurement procedure starts, the request must
/// not be sent, since its transaction is cancelled before the procedure even gets to send it.
TEST_F(f1ap_cu_positioning_measurement_test, when_f1ap_already_stopped_then_request_is_not_sent)
{
  async_task<void>         stop_task = f1ap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);
  ASSERT_TRUE(stop_task.ready());

  start_procedure(make_request());

  ASSERT_FALSE(was_request_sent());
  ASSERT_TRUE(t.ready());
  EXPECT_FALSE(t.get().has_value());
}
