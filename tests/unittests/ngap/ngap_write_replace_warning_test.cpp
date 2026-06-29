// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class ngap_write_replace_warning_test : public ngap_test
{
protected:
  bool was_request_forwarded_to_cu_cp() const { return cu_cp_notifier.last_write_replace_warning_request.has_value(); }

  bool was_response_sent_to_amf() const
  {
    if (n2_gw.last_ngap_msgs.empty()) {
      return false;
    }
    const auto& last = n2_gw.last_ngap_msgs.back();
    return last.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::successful_outcome &&
           last.pdu.successful_outcome().value.type() ==
               asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::write_replace_warning_resp;
  }

  const asn1::ngap::write_replace_warning_resp_ies_container& last_response_ies() const
  {
    return *n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.write_replace_warning_resp();
  }
};

/// Valid minimal request is forwarded to CU-CP and a response is sent to the AMF.
TEST_F(ngap_write_replace_warning_test, when_minimal_request_received_request_is_forwarded_and_response_is_sent)
{
  ngap_message msg = generate_write_replace_warning_request();
  ngap->handle_message(msg);
  ctrl_worker.run_pending_tasks();

  ASSERT_TRUE(was_request_forwarded_to_cu_cp());
  ASSERT_TRUE(was_response_sent_to_amf());
}

/// Mandatory IEs are correctly decoded into the common type.
TEST_F(ngap_write_replace_warning_test, when_minimal_request_received_mandatory_ies_are_correctly_decoded)
{
  ngap_message msg = generate_write_replace_warning_request();
  ngap->handle_message(msg);
  ctrl_worker.run_pending_tasks();

  ASSERT_TRUE(was_request_forwarded_to_cu_cp());
  const auto& req = cu_cp_notifier.last_write_replace_warning_request.value();
  EXPECT_EQ(req.msg_id, 0x1234);
  EXPECT_EQ(req.serial_num, 0x0001);
  EXPECT_EQ(req.repeat_period, 4U);
  EXPECT_EQ(req.nof_broadcasts_requested, 2U);
  EXPECT_FALSE(req.warning_area_list.has_value());
  EXPECT_FALSE(req.warning_type.has_value());
  EXPECT_FALSE(req.data_coding_scheme.has_value());
  EXPECT_FALSE(req.warning_msg_contents.has_value());
  EXPECT_FALSE(req.concurrent_warning_msg_ind);
}

/// Optional IEs are correctly decoded when present.
TEST_F(ngap_write_replace_warning_test, when_request_with_optionals_received_optional_ies_are_correctly_decoded)
{
  ngap_message msg = generate_write_replace_warning_request_with_optionals();
  ngap->handle_message(msg);
  ctrl_worker.run_pending_tasks();

  ASSERT_TRUE(was_request_forwarded_to_cu_cp());
  const auto& req = cu_cp_notifier.last_write_replace_warning_request.value();

  // Warning Type.
  ASSERT_TRUE(req.warning_type.has_value());
  EXPECT_EQ(req.warning_type.value(), 0x0180U);

  // Data Coding Scheme.
  ASSERT_TRUE(req.data_coding_scheme.has_value());
  EXPECT_EQ(req.data_coding_scheme.value(), 0x0f);

  // Warning Message Contents.
  ASSERT_TRUE(req.warning_msg_contents.has_value());
  ASSERT_EQ(req.warning_msg_contents.value().length(), 3U);
  EXPECT_EQ(req.warning_msg_contents.value()[0], 0xaa);
  EXPECT_EQ(req.warning_msg_contents.value()[1], 0xbb);
  EXPECT_EQ(req.warning_msg_contents.value()[2], 0xcc);

  // Warning Area List (NR CGI).
  ASSERT_TRUE(req.warning_area_list.has_value());
  ASSERT_TRUE(std::holds_alternative<ngap_nr_cgi_list_for_warning>(req.warning_area_list.value()));
  const auto& cgi_list = std::get<ngap_nr_cgi_list_for_warning>(req.warning_area_list.value());
  ASSERT_EQ(cgi_list.size(), 1U);
  EXPECT_EQ(cgi_list[0].plmn_id, plmn_identity::test_value());

  // Concurrent Warning Message Indication.
  EXPECT_TRUE(req.concurrent_warning_msg_ind);
}

/// Response echoes back the msg_id and serial_num from the request.
TEST_F(ngap_write_replace_warning_test, when_response_sent_msg_id_and_serial_num_are_echoed_back)
{
  // Configure the notifier to return a specific response.
  cu_cp_notifier.write_replace_warning_response.msg_id     = 0x1234;
  cu_cp_notifier.write_replace_warning_response.serial_num = 0x0001;

  ngap_message msg = generate_write_replace_warning_request();
  ngap->handle_message(msg);
  ctrl_worker.run_pending_tasks();

  ASSERT_TRUE(was_response_sent_to_amf());
  const auto& resp = last_response_ies();
  EXPECT_EQ(resp.msg_id.to_number(), 0x1234U);
  EXPECT_EQ(resp.serial_num.to_number(), 0x0001U);
}
