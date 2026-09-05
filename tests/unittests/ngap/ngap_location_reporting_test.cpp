// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/cu_cp_location_reporting_types.h"
#include "ocudu/ran/cu_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class ngap_location_reporting_test : public ngap_test
{
protected:
  cu_cp_ue_index_t start_procedure(rnti_t rnti = rnti_t::MIN_CRNTI) { return create_ue(rnti); }

  bool was_location_report_forwarded() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::location_report;
  }

  bool was_location_reporting_failure_indication_forwarded() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::location_report_fail_ind;
  }

  bool was_error_indication_sent() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }

  bool was_ue_release_requested(const test_ue& ue) const { return cu_cp_notifier.last_command.ue_index == ue.ue_index; }
};

TEST_F(ngap_location_reporting_test,
       when_ngap_receives_location_reporting_control_message_from_amf_it_forwards_it_to_cucp)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue     = test_ues.at(ue_index);
  ue.amf_ue_id = uint_to_amf_ue_id(
      test_rng::uniform_int<uint64_t>(to_underlying(amf_ue_id_t::min), to_underlying(amf_ue_id_t::max)));

  // Check that initial UE message is sent to AMF and that UE objects has been created.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // Send location reporting control message from AMF
  ngap_message location_reporting_control_msg =
      generate_location_reporting_control_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());

  ngap->handle_message(location_reporting_control_msg);

  // Check that CU-CP notifier was called with correct UE index and report type.
  ASSERT_TRUE(cu_cp_notifier.last_location_reporting_ctrl_ue_index.has_value());
  ASSERT_EQ(cu_cp_notifier.last_location_reporting_ctrl_ue_index.value(), ue_index);
  ASSERT_TRUE(cu_cp_notifier.last_location_reporting_ctrl.has_value());
  ASSERT_EQ(cu_cp_notifier.last_location_reporting_ctrl.value().location_reporting_type,
            location_report_request::event_type::direct);
}

TEST_F(ngap_location_reporting_test,
       when_ngap_receives_location_reporting_failure_indication_message_from_cucp_it_forwards_it_to_amf)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Check that initial UE message is sent to AMF and that UE objects has been created.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // Inject DL NAS Transport to assign AMF UE ID in the NGAP UE context.
  run_dl_nas_transport(ue_index);

  // Send NGAP Location Reporting Failure Indication message
  location_report_failure_indication location_report_failure_ind = {};
  location_report_failure_ind.ue_index                           = ue_index;
  location_report_failure_ind.cause = ngap_cause_radio_network_t::multiple_location_report_ref_id_instances;
  ngap->handle_location_reporting_failure_indication_transmission(location_report_failure_ind);

  // Check that AMF notifier received the location reporting failure indication message.
  ASSERT_TRUE(was_location_reporting_failure_indication_forwarded());
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report_fail_ind()->amf_ue_ngap_id,
            to_underlying(ue.amf_ue_id.value()));
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report_fail_ind()->ran_ue_ngap_id,
            to_underlying(ue.ran_ue_id.value()));
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report_fail_ind()->cause.type(),
            asn1::ngap::cause_c::types_opts::radio_network);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report_fail_ind()->cause.radio_network().value,
            asn1::ngap::cause_radio_network_opts::multiple_location_report_ref_id_instances);
}

TEST_F(ngap_location_reporting_test, when_ngap_receives_location_report_message_from_cucp_it_forwards_it_to_amf)
{
  ASSERT_EQ(ngap->get_nof_ues(), 0);

  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Check that initial UE message is sent to AMF and that UE objects has been created.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap->get_nof_ues(), 1);

  // Inject DL NAS Transport to assign AMF UE ID in the NGAP UE context.
  run_dl_nas_transport(ue_index);

  // Send NGAP Location Reporting message
  location_report location_report                   = {};
  location_report.ue_index                          = ue_index;
  location_report.user_location_info.nr_cgi.plmn_id = plmn_identity::test_value();
  location_report.user_location_info.nr_cgi.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  location_report.user_location_info.tai.plmn_id    = plmn_identity::test_value();
  location_report.user_location_info.tai.tac        = 7;
  ngap->handle_location_report_transmission(location_report);

  // Check that AMF notifier received the location report message.
  ASSERT_TRUE(was_location_report_forwarded());
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report()->amf_ue_ngap_id,
            to_underlying(ue.amf_ue_id.value()));
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report()->ran_ue_ngap_id,
            to_underlying(ue.ran_ue_id.value()));
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report()->location_report_request_type.event_type,
            asn1::ngap::event_type_opts::options::direct);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back()
                .pdu.init_msg()
                .value.location_report()
                ->location_report_request_type.report_area.value,
            asn1::ngap::report_area_opts::options::cell);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report()->user_location_info.type(),
            asn1::ngap::user_location_info_c::types_opts::options::user_location_info_nr);
  const auto& user_location_info_nr =
      n2_gw.last_ngap_msgs.back().pdu.init_msg().value.location_report()->user_location_info.user_location_info_nr();
  ASSERT_EQ(user_location_info_nr.nr_cgi.nr_cell_id.to_number(),
            nr_cell_identity::create(gnb_id_t{411, 22}, 0).value().value());
  ASSERT_EQ(user_location_info_nr.nr_cgi.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
  ASSERT_EQ(user_location_info_nr.tai.plmn_id.to_number(), plmn_identity::test_value().to_bcd());
  ASSERT_EQ(user_location_info_nr.tai.tac.to_number(), 7);
}

/// Test UE ID pair consistency.
TEST_F(ngap_location_reporting_test,
       when_amf_ue_id_pair_is_incosistent_then_old_ue_is_released_and_error_indication_is_sent)
{
  // Test preamble - create two UEs.
  cu_cp_ue_index_t ue_index1 = this->start_procedure();
  cu_cp_ue_index_t ue_index2 = this->start_procedure(to_rnti(0x2));

  auto&    ue1 = test_ues.at(ue_index1);
  auto&    ue2 = test_ues.at(ue_index2);
  unsigned amf_id{};

  // Give UE1 an AMF UE ID via DL NAS transport.
  ngap_message dl_nas_transport =
      generate_downlink_nas_transport_message(uint_to_amf_ue_id(amf_id), ue1.ran_ue_id.value());
  ngap->handle_message(dl_nas_transport);

  // Send Location Reporting Control with the same AMF UE ID but UE2's RAN UE ID.
  ngap_message location_reporting_control_msg =
      generate_location_reporting_control_message(uint_to_amf_ue_id(amf_id), ue2.ran_ue_id.value());
  ngap->handle_message(location_reporting_control_msg);

  // Check that release of old UE has been requested.
  ASSERT_TRUE(was_ue_release_requested(ue1));

  // Check that error indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.error_ind()->cause.radio_network(),
            asn1::ngap::cause_radio_network_e::options::inconsistent_remote_ue_ngap_id);
}

/// Test UE ID correctness.
TEST_F(ngap_location_reporting_test, when_location_report_ue_index_is_invalid_message_is_dropped)
{
  // Record the number of messages sent to AMF before the test.
  size_t n_msgs_before = n2_gw.last_ngap_msgs.size();

  // Send location report with invalid UE index.
  location_report location_report                   = {};
  location_report.ue_index                          = cu_cp_ue_index_t::invalid;
  location_report.user_location_info.nr_cgi.plmn_id = plmn_identity::test_value();
  location_report.user_location_info.nr_cgi.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  location_report.user_location_info.tai.plmn_id    = plmn_identity::test_value();
  location_report.user_location_info.tai.tac        = 7;
  ngap->handle_location_report_transmission(location_report);

  // Verify no new message was sent to AMF.
  ASSERT_EQ(n2_gw.last_ngap_msgs.size(), n_msgs_before);
}
