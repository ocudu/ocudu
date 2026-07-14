// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/ngap/ngap_test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/ngap/ngap_handover.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Test successful handover preparation procedure.
TEST_F(ngap_test, when_source_gnb_handover_preparation_triggered_then_ho_command_received)
{
  // Setup UE context
  cu_cp_ue_index_t ue_index = create_ue();
  run_dl_nas_transport(ue_index); // needed to allocate AMF UE id.

  // Manually add existing PDU sessions to UP manager.
  add_pdu_session_to_up_manager(
      ue_index,
      uint_to_pdu_session_id(1),
      pdu_session_type_t::ipv4,
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), int_to_gtpu_teid(1)},
      uint_to_drb_id(1),
      uint_to_qos_flow_id(0));

  auto& ue = test_ues.at(ue_index);
  ue.rrc_ue_handler.set_ho_preparation_message({});

  ngap_handover_preparation_request request =
      generate_handover_preparation_request(ue_index,
                                            ue_mng.find_ue(ue_index)->get_up_resource_manager().get_pdu_sessions_map(),
                                            nr_cell_identity::create({1, 22}, 1).value(),
                                            22);

  // Action 1: Launch HO preparation procedure
  test_logger.info("Launch source NGAP handover preparation procedure");
  async_task<ngap_handover_preparation_response>         t = ngap->handle_handover_preparation_request(request);
  lazy_task_launcher<ngap_handover_preparation_response> t_launcher(t);

  // Status: AMF received Handover Required.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_required);

  ASSERT_FALSE(t.ready());

  // Inject Handover Command.
  ngap_message ho_cmd = generate_valid_handover_command(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(ho_cmd);

  // Procedure should have succeeded.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

/// Test that the Handover Required correctly encodes the target PLMN in the TargetID's Global gNB-ID and in the
/// Source-to-Target Transparent Container's Target Cell ID, for a target cell belonging to a different PLMN than the
/// one currently serving the UE.
TEST_F(ngap_test, when_target_plmn_differs_from_serving_plmn_then_handover_required_uses_target_plmn)
{
  // Setup UE context. The UE is served under the test PLMN (see ngap_test constructor and create_ue()).
  cu_cp_ue_index_t ue_index = create_ue();
  run_dl_nas_transport(ue_index); // needed to allocate AMF UE id.

  // Manually add existing PDU sessions to UP manager (Handover Required requires at least one).
  add_pdu_session_to_up_manager(
      ue_index,
      uint_to_pdu_session_id(1),
      pdu_session_type_t::ipv4,
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), int_to_gtpu_teid(1)},
      uint_to_drb_id(1),
      uint_to_qos_flow_id(0));

  auto& ue = test_ues.at(ue_index);
  ue.rrc_ue_handler.set_ho_preparation_message({});

  // Target cell belongs to a different PLMN than the one currently serving the UE.
  const plmn_identity serving_plmn = plmn_identity::test_value();
  const plmn_identity target_plmn  = plmn_identity::parse("00202").value();
  ASSERT_NE(target_plmn, serving_plmn);

  ngap_handover_preparation_request request =
      generate_handover_preparation_request(ue_index,
                                            ue_mng.find_ue(ue_index)->get_up_resource_manager().get_pdu_sessions_map(),
                                            nr_cell_identity::create({1, 22}, 1).value(),
                                            22,
                                            target_plmn);

  // Action: Launch HO preparation procedure.
  async_task<ngap_handover_preparation_response>         t = ngap->handle_handover_preparation_request(request);
  lazy_task_launcher<ngap_handover_preparation_response> t_launcher(t);

  // Status: AMF received Handover Required.
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_required);

  const asn1::ngap::ho_required_s& ho_required = n2_gw.last_ngap_msgs.back().pdu.init_msg().value.ho_required();

  // TargetID -> targetRANNodeID -> globalRANNodeID -> globalGNB-ID -> pLMNIdentity must be the target PLMN.
  const auto& target_ran_node_id = ho_required->target_id.target_ran_node_id();
  EXPECT_EQ(target_ran_node_id.global_ran_node_id.global_gnb_id().plmn_id.to_bytes(), target_plmn.to_bytes());
  EXPECT_EQ(target_ran_node_id.global_ran_node_id.global_gnb_id().gnb_id.gnb_id().to_number(),
            request.target_id.gnb_id.id);

  // TargetID -> targetRANNodeID -> selectedTAI -> pLMNIdentity must also be the target PLMN.
  EXPECT_EQ(target_ran_node_id.sel_tai.plmn_id.to_bytes(), target_plmn.to_bytes());

  // Source-to-Target Transparent Container's Target Cell ID must carry the target PLMN as well.
  asn1::cbit_ref bref(ho_required->source_to_target_transparent_container);
  asn1::ngap::source_ngran_node_to_target_ngran_node_transparent_container_s transparent_container;
  ASSERT_EQ(transparent_container.unpack(bref), asn1::OCUDUASN_SUCCESS);
  EXPECT_EQ(transparent_container.target_cell_id.nr_cgi().plmn_id.to_bytes(), target_plmn.to_bytes());
}
