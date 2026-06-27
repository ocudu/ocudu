// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_du_test_helpers.h"
#include "lib/du/du_high/du_manager/converters/asn1_ref_time_r16_helpers.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class dummy_f1ap_du_time_provider : public f1ap_du_time_provider
{
public:
  std::optional<f1ap_du_slot_time_info> next_mapping;

  std::optional<f1ap_du_slot_time_info> get_last_mapping(subcarrier_spacing scs) override { return next_mapping; }
};

namespace {

f1ap_message make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::options event_type,
                                            uint16_t                             transaction_id    = 0,
                                            uint16_t                             period_rf         = 0,
                                            bool                                 period_rf_present = false)
{
  f1ap_message msg{};
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_REF_TIME_INFO_REPORT_CTRL);
  auto& ctrl           = msg.pdu.init_msg().value.ref_time_info_report_ctrl();
  ctrl->transaction_id = transaction_id;
  auto& rrt            = ctrl->report_request_type;
  rrt.event_type.value = event_type;
  if (period_rf_present) {
    rrt.report_periodicity_value_present = true;
    rrt.report_periodicity_value         = period_rf;
  }
  return msg;
}

const std::chrono::system_clock::time_point kTestTimePoint =
    std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL}};
constexpr bool kTestIsLocalClock = true;

} // namespace

class f1ap_du_ref_time_info_report_test : public f1ap_du_test
{
public:
  f1ap_du_ref_time_info_report_test()
  {
    f1ap_du_cfg_handler.connect_time_provider(time_provider);
    run_f1_setup_procedure();
    f1c_gw.clear_tx_pdus();

    // Set a fixed mapping: SFN 42, slot 0, kHz15; ref_time_r16 pre-encoded the way the real
    // f1ap_du_mac_time_provider_adapter would.
    f1ap_du_slot_time_info mapping;
    mapping.ref_slot           = slot_point{subcarrier_spacing::kHz15, 42, 0};
    mapping.ref_time_r16       = pack_ref_time_r16(kTestTimePoint, kTestIsLocalClock);
    time_provider.next_mapping = mapping;
  }

  dummy_f1ap_du_time_provider time_provider;
};

TEST_F(f1ap_du_ref_time_info_report_test, on_demand_report_is_sent)
{
  f1ap_message ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::on_demand, 7);
  f1ap->handle_message(ctrl);

  ASSERT_TRUE(f1c_gw.tx_pdus_sent());
  const f1ap_message& report = f1c_gw.last_tx_pdu();
  ASSERT_EQ(report.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(report.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ref_time_info_report);
  const auto& rep = report.pdu.init_msg().value.ref_time_info_report();
  EXPECT_EQ(rep->transaction_id, 7U);
  EXPECT_EQ(rep->time_ref_info.ref_sfn, 42U);
  EXPECT_TRUE(rep->time_ref_info.time_info_type_present);

  // The ref_time octet string must pass through unchanged from the time provider.
  EXPECT_EQ(rep->time_ref_info.ref_time, pack_ref_time_r16(kTestTimePoint, kTestIsLocalClock));
}

TEST_F(f1ap_du_ref_time_info_report_test, on_demand_when_no_mapping_no_report_sent)
{
  time_provider.next_mapping = std::nullopt;

  f1ap_message ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::on_demand);
  f1ap->handle_message(ctrl);

  ASSERT_FALSE(f1c_gw.tx_pdus_sent());
}

TEST_F(f1ap_du_ref_time_info_report_test, periodic_report_fires_on_timer_expiry)
{
  // 1 radio frame = 10 ms; period = 2 radio frames = 20 ms
  f1ap_message ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::periodic, 0, 2, true);
  f1ap->handle_message(ctrl);

  // Timer hasn't fired yet - no report sent immediately.
  ASSERT_FALSE(f1c_gw.tx_pdus_sent());

  // Advance timer by 20 ms (2 radio frames) - first report should be sent.
  for (int i = 0; i < 20; ++i) {
    tick();
  }
  ASSERT_TRUE(f1c_gw.tx_pdus_sent());
  EXPECT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ref_time_info_report);

  f1c_gw.clear_tx_pdus();

  // Second period - another report.
  for (int i = 0; i < 20; ++i) {
    tick();
  }
  ASSERT_TRUE(f1c_gw.tx_pdus_sent());
}

TEST_F(f1ap_du_ref_time_info_report_test, stop_cancels_periodic_reporting)
{
  // Start periodic with period = 1 radio frame.
  f1ap_message start_ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::periodic, 0, 1, true);
  f1ap->handle_message(start_ctrl);

  // Stop before any timer fires.
  f1ap_message stop_ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::stop);
  f1ap->handle_message(stop_ctrl);

  // Advance well past the timer period - no reports expected.
  for (int i = 0; i < 50; ++i) {
    tick();
  }
  ASSERT_FALSE(f1c_gw.tx_pdus_sent());
}

TEST_F(f1ap_du_ref_time_info_report_test, periodic_replaces_running_periodic)
{
  // Start with period = 10 rf (100 ms).
  f1ap_message ctrl1 = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::periodic, 0, 10, true);
  f1ap->handle_message(ctrl1);

  // Advance 50 ms (half the first period) - no report yet.
  for (int i = 0; i < 50; ++i) {
    tick();
  }
  ASSERT_FALSE(f1c_gw.tx_pdus_sent());

  // Replace with a shorter period = 2 rf (20 ms); the old timer is cancelled.
  f1ap_message ctrl2 = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::periodic, 1, 2, true);
  f1ap->handle_message(ctrl2);
  f1c_gw.clear_tx_pdus();

  // New period fires after 20 ms; the old 50 ms remainder must not trigger anything before that.
  for (int i = 0; i < 20; ++i) {
    tick();
  }
  ASSERT_TRUE(f1c_gw.tx_pdus_sent());
  EXPECT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.ref_time_info_report()->transaction_id, 1U);
  f1c_gw.clear_tx_pdus();

  // Advance well past the old timer's original 100 ms deadline (elapsed since ctrl1 = 50 + 20 + 60 = 130 ms).
  // The old timer's deadline (100 ms) does not align with the new 20 ms schedule (which fires again at
  // 90, 110 and 130 ms), so a leaked old timer would show up here as a 4th report carrying the old
  // transaction_id (0).
  for (int i = 0; i < 60; ++i) {
    tick();
  }
  unsigned                    reports_seen = 0;
  std::optional<f1ap_message> pdu          = f1c_gw.pop_tx_pdu();
  while (pdu) {
    EXPECT_EQ(pdu->pdu.init_msg().value.ref_time_info_report()->transaction_id, 1U);
    ++reports_seen;
    pdu = f1c_gw.pop_tx_pdu();
  }
  EXPECT_EQ(reports_seen, 3U);
}

TEST_F(f1ap_du_ref_time_info_report_test, on_demand_stops_running_periodic)
{
  // Start periodic with period = 2 rf (20 ms).
  f1ap_message start_ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::periodic, 0, 2, true);
  f1ap->handle_message(start_ctrl);

  // Send on-demand before the timer fires.
  f1ap_message on_demand_ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::on_demand, 5);
  f1ap->handle_message(on_demand_ctrl);

  // Exactly one report must have been sent immediately with the on-demand transaction_id.
  ASSERT_TRUE(f1c_gw.tx_pdus_sent());
  EXPECT_EQ(f1c_gw.last_tx_pdu().pdu.init_msg().value.ref_time_info_report()->transaction_id, 5U);
  f1c_gw.clear_tx_pdus();

  // No further reports after advancing well past the original period.
  for (int i = 0; i < 50; ++i) {
    tick();
  }
  ASSERT_FALSE(f1c_gw.tx_pdus_sent());
}

TEST_F(f1ap_du_ref_time_info_report_test, periodic_without_periodicity_value_sends_no_report)
{
  // Periodic ctrl with report_periodicity_value_present=false - procedure must ignore it.
  f1ap_message ctrl = make_ref_time_info_report_ctrl(asn1::f1ap::event_type_opts::periodic, 0, 0, false);
  f1ap->handle_message(ctrl);

  ASSERT_FALSE(f1c_gw.tx_pdus_sent());

  // No timer should be running - no fires after advancing time.
  for (int i = 0; i < 50; ++i) {
    tick();
  }
  ASSERT_FALSE(f1c_gw.tx_pdus_sent());
}
