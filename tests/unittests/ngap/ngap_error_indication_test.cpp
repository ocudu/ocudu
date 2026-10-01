// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "ocudu/adt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class ngap_error_indication_test : public ngap_test
{
protected:
  cu_cp_ue_index_t start_procedure()
  {
    cu_cp_ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF.
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC.
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup Request.
    run_initial_context_setup(ue_index);

    return ue_index;
  }

  bool was_error_indication_sent() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }

  bool was_ue_release_requested() const
  {
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request;
  }
};

/// Test handling of error indication message for inexisting UE.
TEST_F(ngap_error_indication_test,
       when_error_indication_message_for_inexisting_ue_received_message_is_dropped_and_error_indication_is_sent)
{
  // Inject error indication message
  ngap_message error_indication_msg = generate_error_indication_message(uint_to_amf_ue_id(10), uint_to_ran_ue_id(0));
  ngap->handle_message(error_indication_msg);

  // Check that Error Indication has been sent to AMF.
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test handling of error indication message for existing UE.
TEST_F(ngap_error_indication_test, when_error_indication_message_for_existing_ue_received_message_is_logged)
{
  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  // Inject error indication message.
  ngap_message error_indication_msg = generate_error_indication_message(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(error_indication_msg);

  ASSERT_TRUE(was_ue_release_requested());
}

/// Test handling of error indication message for existing UE.
TEST_F(
    ngap_error_indication_test,
    when_error_indication_message_with_cause_unknown_local_ue_ngap_id_for_existing_ue_is_received_then_ue_is_released_locally)
{
  // Test preamble.
  cu_cp_ue_index_t ue_index = this->start_procedure();

  auto& ue = test_ues.at(ue_index);

  unsigned nof_n2_msgs_before = n2_gw.last_ngap_msgs.size();

  // Inject error indication message.
  ngap_message error_indication_msg = generate_error_indication_message(
      ue.amf_ue_id.value(), ue.ran_ue_id.value(), ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
  ngap->handle_message(error_indication_msg);

  // No NGAP message should be sent.
  ASSERT_EQ(n2_gw.last_ngap_msgs.size(), nof_n2_msgs_before);
}
