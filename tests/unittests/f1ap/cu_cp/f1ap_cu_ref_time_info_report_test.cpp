// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_cu_test_helpers.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class f1ap_cu_ref_time_info_report_test : public f1ap_cu_test
{};

TEST_F(f1ap_cu_ref_time_info_report_test, send_on_demand_ctrl_produces_correct_pdu)
{
  f1ap_ref_time_report_ctrl_request req;
  req.event_type = f1ap_ref_time_event_type::on_demand;

  f1ap->get_f1ap_interface_management_handler().handle_ref_time_info_report_ctrl(req);

  const f1ap_message& sent = f1ap_pdu_notifier.last_f1ap_msg;
  ASSERT_EQ(sent.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(sent.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ref_time_info_report_ctrl);

  const auto& ctrl = sent.pdu.init_msg().value.ref_time_info_report_ctrl();
  EXPECT_EQ(ctrl->report_request_type.event_type.value, asn1::f1ap::event_type_opts::on_demand);
  EXPECT_FALSE(ctrl->report_request_type.report_periodicity_value_present);
}

TEST_F(f1ap_cu_ref_time_info_report_test, send_periodic_ctrl_includes_periodicity)
{
  f1ap_ref_time_report_ctrl_request req;
  req.event_type            = f1ap_ref_time_event_type::periodic;
  req.report_periodicity_rf = 10U;

  f1ap->get_f1ap_interface_management_handler().handle_ref_time_info_report_ctrl(req);

  const auto& ctrl = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.ref_time_info_report_ctrl();
  EXPECT_EQ(ctrl->report_request_type.event_type.value, asn1::f1ap::event_type_opts::periodic);
  EXPECT_TRUE(ctrl->report_request_type.report_periodicity_value_present);
  EXPECT_EQ(ctrl->report_request_type.report_periodicity_value, 10U);
}

TEST_F(f1ap_cu_ref_time_info_report_test, send_stop_ctrl_produces_correct_pdu)
{
  f1ap_ref_time_report_ctrl_request req;
  req.event_type = f1ap_ref_time_event_type::stop;

  f1ap->get_f1ap_interface_management_handler().handle_ref_time_info_report_ctrl(req);

  const auto& ctrl = f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.ref_time_info_report_ctrl();
  EXPECT_EQ(ctrl->report_request_type.event_type.value, asn1::f1ap::event_type_opts::stop);
}

TEST_F(f1ap_cu_ref_time_info_report_test, incoming_report_triggers_notifier_callback)
{
  // Build a ReferenceTimeInformationReport PDU with a known SFN. The ref_time content is opaque to
  // F1AP, so an arbitrary placeholder byte buffer is used here.
  byte_buffer ref_time_r16 = byte_buffer::create({0x1, 0x2, 0x3, 0x4}).value();

  f1ap_message msg{};
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_REF_TIME_INFO_REPORT);
  auto& report                                 = msg.pdu.init_msg().value.ref_time_info_report();
  report->transaction_id                       = 3;
  report->time_ref_info.ref_time               = ref_time_r16.copy();
  report->time_ref_info.ref_sfn                = 77U;
  report->time_ref_info.time_info_type_present = true;

  f1ap->handle_message(msg);

  ASSERT_TRUE(du_processor_notifier.last_ref_time_info_report.has_value());
  EXPECT_EQ(du_processor_notifier.last_ref_time_info_report->ref_slot.sfn(), 77U);
  EXPECT_TRUE(du_processor_notifier.last_ref_time_info_report->is_local_clock);
  EXPECT_EQ(du_processor_notifier.last_ref_time_info_report->ref_time_r16, ref_time_r16);
}

TEST_F(f1ap_cu_ref_time_info_report_test, incoming_report_uncertainty_is_forwarded)
{
  f1ap_message msg{};
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_REF_TIME_INFO_REPORT);
  auto& report                              = msg.pdu.init_msg().value.ref_time_info_report();
  report->transaction_id                    = 4;
  report->time_ref_info.ref_time            = byte_buffer::create({0x1}).value();
  report->time_ref_info.ref_sfn             = 10U;
  report->time_ref_info.uncertainty_present = true;
  report->time_ref_info.uncertainty         = 100U;

  f1ap->handle_message(msg);

  ASSERT_TRUE(du_processor_notifier.last_ref_time_info_report.has_value());
  ASSERT_TRUE(du_processor_notifier.last_ref_time_info_report->uncertainty.has_value());
  EXPECT_EQ(*du_processor_notifier.last_ref_time_info_report->uncertainty, 100U);
}
