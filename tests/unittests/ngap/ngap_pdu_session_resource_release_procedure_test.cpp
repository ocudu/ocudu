// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class ngap_pdu_session_resource_release_procedure_test : public ngap_test
{
protected:
  cu_cp_ue_index_t start_procedure(const pdu_session_id_t pdu_session_id, bool enable_security = true)
  {
    cu_cp_ue_index_t ue_index = create_ue();

    // Inject DL NAS transport message from AMF.
    run_dl_nas_transport(ue_index);

    // Inject UL NAS transport message from RRC.
    run_ul_nas_transport(ue_index);

    // Inject Initial Context Setup request.
    run_initial_context_setup(ue_index);

    // Inject PDU Session Resource Setup request.
    run_pdu_session_resource_setup(ue_index, pdu_session_id);

    if (enable_security) {
      if (!enable_ue_security(ue_index)) {
        return cu_cp_ue_index_t::invalid;
      }
    }

    return ue_index;
  }

  bool was_conversion_successful(ngap_message     pdu_session_resource_release_command,
                                 pdu_session_id_t pdu_session_id) const
  {
    bool test_1 = pdu_session_resource_release_command.pdu.init_msg()
                      .value.pdu_session_res_release_cmd()
                      ->pdu_session_res_to_release_list_rel_cmd.size() ==
                  cu_cp_notifier.last_release_command.pdu_session_res_to_release_list_rel_cmd.size();

    bool test_2 =
        cu_cp_notifier.last_release_command.pdu_session_res_to_release_list_rel_cmd[pdu_session_id].pdu_session_id ==
        pdu_session_id;

    return test_1 && test_2;
  }

  bool was_pdu_session_resource_release_command_valid() const
  {
    // Check that AMF notifier was called with right type.
    return n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
           asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_release_resp;
  }

  bool was_pdu_session_resource_setup_request_valid() const
  {
    // Check that AMF notifier was called with right type.
    bool test_1 = n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type() ==
                  asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp;

    // Check that response contains PDU Session Resource Setup List.
    bool test_2 = n2_gw.last_ngap_msgs.back()
                      .pdu.successful_outcome()
                      .value.pdu_session_res_setup_resp()
                      ->pdu_session_res_setup_list_su_res_present;

    return test_1 && test_2;
  }

  bool was_error_indication_sent() const
  {
    // Check that AMF notifier was called with right type.
    return n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type() ==
           asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::error_ind;
  }
};

/// Test PDU Session Resource Release Command.
TEST_F(ngap_pdu_session_resource_release_procedure_test,
       when_pdu_sessions_to_release_not_found_then_pdu_session_release_fails)
{
  // Test preamble
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rng::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  cu_cp_ue_index_t ue_index = this->start_procedure(pdu_session_id);

  auto& ue = test_ues.at(ue_index);

  // Set CU-CP outcome to response with empty released PDU sessions list, which should trigger transmission of error
  // indication.
  cu_cp_notifier.set_pdu_session_resource_release_outcome(
      ngap_pdu_session_resource_release_response{.released_pdu_sessions = {}});

  ngap_message pdu_session_resource_release_command =
      generate_valid_pdu_session_resource_release_command(ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_release_command);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_release_command, pdu_session_id));

  // Check that an error indication was sent.
  ASSERT_TRUE(was_error_indication_sent());
}

/// Test PDU Session Resource Release Command.
TEST_F(ngap_pdu_session_resource_release_procedure_test,
       when_valid_pdu_session_resource_release_command_received_then_pdu_session_release_succeeds)
{
  // Test preamble
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rng::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  cu_cp_ue_index_t ue_index = this->start_procedure(pdu_session_id);

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_release_command =
      generate_valid_pdu_session_resource_release_command(ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_release_command);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_release_command, pdu_session_id));

  // Check that PDU Session Resource Release Command was valid.
  ASSERT_TRUE(was_pdu_session_resource_release_command_valid());
}

/// Test PDU Session Resource Setup after PDU Session Resource Release Command.
TEST_F(ngap_pdu_session_resource_release_procedure_test,
       when_pdu_session_resource_setup_request_received_after_release_command_then_pdu_session_setup_succeeds)
{
  // Test preamble.
  pdu_session_id_t pdu_session_id = uint_to_pdu_session_id(test_rng::uniform_int<uint16_t>(
      pdu_session_id_to_uint(pdu_session_id_t::min), pdu_session_id_to_uint(pdu_session_id_t::max)));

  cu_cp_ue_index_t ue_index = this->start_procedure(pdu_session_id);

  auto& ue = test_ues.at(ue_index);

  ngap_message pdu_session_resource_release_command =
      generate_valid_pdu_session_resource_release_command(ue.amf_ue_id.value(), ue.ran_ue_id.value(), pdu_session_id);
  ngap->handle_message(pdu_session_resource_release_command);

  // Check conversion in adapter.
  ASSERT_TRUE(was_conversion_successful(pdu_session_resource_release_command, pdu_session_id));

  // Check that PDU Session Resource Release Command was valid.
  ASSERT_TRUE(was_pdu_session_resource_release_command_valid());

  // Inject PDU Session Resource Setup Request with the PDU Session ID that has just been released.
  run_pdu_session_resource_setup(ue_index, pdu_session_id);
  // Check that PDU Session Resource Setup Request was invalid.
  ASSERT_TRUE(was_pdu_session_resource_setup_request_valid());
}
