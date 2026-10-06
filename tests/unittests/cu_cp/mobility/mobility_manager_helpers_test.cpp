// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/mobility_manager/mobility_manager_helpers.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

const pdu_session_id_t psi  = uint_to_pdu_session_id(1);
const drb_id_t         drb  = drb_id_t::drb1;
const qos_flow_id_t    qfi1 = uint_to_qos_flow_id(1);
const qos_flow_id_t    qfi2 = uint_to_qos_flow_id(2);

// Builds a UE with one PDU session, one DRB and two QoS flows mapped to it.
std::map<pdu_session_id_t, up_pdu_session_context> make_pdu_sessions()
{
  up_pdu_session_context pdu_session{
      psi,
      pdu_session_type_t::ipv4,
      up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), int_to_gtpu_teid(1)}};

  up_drb_context drb_ctxt;
  drb_ctxt.drb_id         = drb;
  drb_ctxt.pdu_session_id = psi;
  drb_ctxt.s_nssai        = s_nssai_t{slice_service_type{1}, slice_differentiator{}};
  for (qos_flow_id_t qfi : {qfi1, qfi2}) {
    drb_ctxt.qos_flows.emplace(qfi, up_qos_flow_context{qfi, {}});
  }
  pdu_session.drbs.emplace(drb, drb_ctxt);

  std::map<pdu_session_id_t, up_pdu_session_context> pdu_sessions;
  pdu_sessions.emplace(psi, pdu_session);
  return pdu_sessions;
}

xnap_handover_request make_handover_request()
{
  return generate_xnap_handover_request(cu_cp_ue_index_t::min,
                                        nr_cell_global_id_t{},
                                        guami_t{},
                                        uint_to_amf_ue_id(0),
                                        transport_layer_address::create_from_string("127.0.0.1"),
                                        aggregate_maximum_bit_rate_t{},
                                        security::security_context{},
                                        make_pdu_sessions(),
                                        byte_buffer{},
                                        std::nullopt);
}

} // namespace

// The source proposes every QoS flow it hands over for DL data forwarding, and leaves it to the target to decide
// which ones it accepts (TS 38.300 section 9.2.3.2.3).
TEST(mobility_manager_helpers_test, when_handover_request_is_generated_then_every_qos_flow_is_proposed_for_forwarding)
{
  const xnap_handover_request request = make_handover_request();

  const auto& pdu_sessions = request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list;
  ASSERT_TRUE(pdu_sessions.contains(psi));
  const cu_cp_pdu_session_res_setup_item& pdu_session = pdu_sessions[psi];

  ASSERT_TRUE(pdu_session.data_forwarding_info_from_source.has_value());
  const cu_cp_data_forwarding_info_from_source& forwarding_info = pdu_session.data_forwarding_info_from_source.value();

  ASSERT_EQ(forwarding_info.qos_flows_to_be_forwarded.size(), 2U);
  for (const cu_cp_qos_flow_to_be_forwarded_item& qos_flow : forwarding_info.qos_flows_to_be_forwarded) {
    EXPECT_TRUE(qos_flow.dl_forwarding.value_or(false));
  }
  EXPECT_EQ(forwarding_info.qos_flows_to_be_forwarded[0].qos_flow_id, qfi1);
  EXPECT_EQ(forwarding_info.qos_flows_to_be_forwarded[1].qos_flow_id, qfi2);
}

// The source reports its own DRB-to-QoS-flow mapping, so that the target can keep its DRB numbering
// (TS 38.423 section 9.2.1.17).
TEST(mobility_manager_helpers_test, when_handover_request_is_generated_then_the_source_drb_mapping_is_reported)
{
  const xnap_handover_request request = make_handover_request();

  const cu_cp_pdu_session_res_setup_item& pdu_session =
      request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list[psi];
  ASSERT_TRUE(pdu_session.data_forwarding_info_from_source.has_value());

  const auto& drb_map = pdu_session.data_forwarding_info_from_source->drbs_to_qos_flows_map_list;
  ASSERT_EQ(drb_map.size(), 1U);
  EXPECT_EQ(drb_map[0].drb_id, drb);
  ASSERT_EQ(drb_map[0].associated_qos_flow_list.size(), 2U);
  EXPECT_EQ(drb_map[0].associated_qos_flow_list[0].qos_flow_id, qfi1);
  EXPECT_EQ(drb_map[0].associated_qos_flow_list[1].qos_flow_id, qfi2);
}

// The QoS flow setup list carries the flows themselves, apart from the forwarding proposal.
TEST(mobility_manager_helpers_test, when_handover_request_is_generated_then_the_qos_flows_are_set_up)
{
  const xnap_handover_request request = make_handover_request();

  const cu_cp_pdu_session_res_setup_item& pdu_session =
      request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list[psi];
  ASSERT_EQ(pdu_session.qos_flow_setup_request_items.size(), 2U);
  EXPECT_TRUE(pdu_session.qos_flow_setup_request_items.contains(qfi1));
  EXPECT_TRUE(pdu_session.qos_flow_setup_request_items.contains(qfi2));
}
