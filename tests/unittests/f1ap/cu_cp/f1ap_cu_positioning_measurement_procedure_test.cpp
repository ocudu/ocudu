// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
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

  unsigned sent_transaction_id() const
  {
    return f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.positioning_meas_request()->transaction_id;
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

TEST_F(f1ap_cu_positioning_measurement_test, when_response_carries_an_unsupported_result_then_its_trp_is_ignored)
{
  start_procedure(make_request());
  ASSERT_TRUE(was_request_sent());

  // Replace the result of the second TRP with a Multiple UL AoA result.
  f1ap_message pdu = test_helpers::generate_positioning_measurement_response_with_aoa(
      lmf_meas_id_t::min, ran_meas_id_t::min, {uint_to_trp_id(1), uint_to_trp_id(2)}, 1800, sent_transaction_id());
  auto& ext = pdu.pdu.successful_outcome()
                  .value.positioning_meas_resp()
                  ->pos_meas_result_list[1]
                  .pos_meas_result[0]
                  .measured_results_value.set_choice_ext();
  ext->set(measured_results_value_ext_ies_o::value_c::types_opts::multiple_ul_ao_a);
  ext->multiple_ul_ao_a().multiple_ul_ao_a.resize(1);
  ext->multiple_ul_ao_a().multiple_ul_ao_a[0].set_ul_ao_a().azimuth_ao_a = 1800;
  f1ap->handle_message(pdu);

  ASSERT_TRUE(t.ready());
  const auto& res = t.get();
  ASSERT_TRUE(res.has_value());
  ASSERT_EQ(res.value().trp_meas_resp_list.size(), 1);
  ASSERT_EQ(res.value().trp_meas_resp_list[0].trp_id, uint_to_trp_id(1));
  ASSERT_EQ(res.value().trp_meas_resp_list[0].meas_result.size(), 1);
}
