// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/rrc/rrc_ue_test_helpers.h"
#include "tests/unittests/xnap/xnap_test_messages.h"
#include "xnap_test_helpers.h"
#include "ocudu/security/security.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/xnap/xnap_handover.h"
#include <chrono>
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class for XNAP handover preparation procedure tests.
class xnap_handover_preparation_procedure_test : public xnap_test
{
public:
  xnap_handover_preparation_procedure_test()           = default;
  ~xnap_handover_preparation_procedure_test() override = default;

  static xnap_handover_request generate_handover_request(cu_cp_ue_index_t            ue_index,
                                                         security::security_context& sec_ctxt)
  {
    xnap_handover_request request;

    request.ue_index = ue_index,
    request.nr_cgi   = nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 1).value()};
    request.guami = guami_t{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1};
    request.ue_context_info_ho_request.amf_ue_id        = amf_ue_id_to_uint(amf_ue_id_t::min);
    request.ue_context_info_ho_request.amf_addr         = transport_layer_address::create_from_string("127.0.0.1");
    request.ue_context_info_ho_request.security_context = sec_ctxt;
    request.ue_context_info_ho_request.ue_ambr.dl       = 0;
    request.ue_context_info_ho_request.ue_ambr.ul       = 0;

    cu_cp_pdu_session_res_setup_item item;
    item.pdu_session_id                            = pdu_session_id_t::min;
    item.s_nssai                                   = s_nssai_t{slice_service_type{1}, slice_differentiator{}};
    item.pdu_session_aggregate_maximum_bit_rate_dl = 100000;
    item.pdu_session_aggregate_maximum_bit_rate_ul = 100000;
    item.ul_ngu_up_tnl_info =
        up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), int_to_gtpu_teid(12345)};
    item.pdu_session_type = pdu_session_type_t::ipv4;

    qos_characteristics           qos_desc(non_dyn_5qi_descriptor{});
    qos_flow_level_qos_parameters qos_params{.qos_desc             = qos_desc,
                                             .alloc_retention_prio = alloc_and_retention_priority{}};

    qos_flow_setup_request_item qos_flow_item{.qos_flow_id               = qos_flow_id_t::min,
                                              .qos_flow_level_qos_params = qos_params};

    item.qos_flow_setup_request_items.emplace(qos_flow_id_t::min, qos_flow_item);

    request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list.emplace(item.pdu_session_id, item);

    return request;
  }

  void set_handover_procedure_outcome(bool outcome) { cu_cp_notifier.set_xnap_handover_request_outcome(outcome); }
};

///////////////////////////////////////////////////////////////////////////////
//                             Source CU-CP
///////////////////////////////////////////////////////////////////////////////

/// Test unsuccessful handover preparation procedure.
TEST_F(xnap_handover_preparation_procedure_test, when_handover_preparation_failure_received_then_procedure_fails)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Create UE context.
  cu_cp_ue_index_t ue_index = create_ue();

  // Generate Security context for the UE.
  security::security_context sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
  xnap_handover_request      request  = generate_handover_request(ue_index, sec_ctxt);

  // Action 1: Launch HO preparation procedure
  logger.info("Launch source XNAP handover preparation procedure");
  async_task<xnap_handover_preparation_response>         t = xnap->handle_handover_request_required(request);
  lazy_task_launcher<xnap_handover_preparation_response> t_launcher(t);

  // Status: XN-C peer received Handover Required.
  ASSERT_EQ(get_last_message().pdu.type().value, asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_request);

  ASSERT_FALSE(t.ready());

  // Inject Handover Preparation Failure.
  xnap_message ho_prep_fail = ::generate_handover_preparation_failure(peer_xnap_ue_id_t::min);
  xnap->handle_message(ho_prep_fail);

  // Procedure should have failed.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

/// Test unsuccessful handover preparation procedure.
TEST_F(xnap_handover_preparation_procedure_test, when_handover_preparation_times_out_then_handover_cancel_is_sent)
{
  // Run XN setup..
  run_xn_setup(xnap_peer_cfg);

  // Create UE context.
  cu_cp_ue_index_t ue_index = create_ue();

  // Generate Security context for the UE.
  security::security_context sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
  xnap_handover_request      request  = generate_handover_request(ue_index, sec_ctxt);

  // Action 1: Launch HO preparation procedure
  logger.info("Launch source XNAP handover preparation procedure");
  async_task<xnap_handover_preparation_response>         t = xnap->handle_handover_request_required(request);
  lazy_task_launcher<xnap_handover_preparation_response> t_launcher(t);

  // Status: XN-C peer received Handover Required.
  ASSERT_EQ(get_last_message().pdu.type().value, asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_request);

  ASSERT_FALSE(t.ready());

  // Status: Fail Handover Preparation procedure (XN-C peer doesn't respond).
  ASSERT_TRUE(this->tick(t, std::chrono::milliseconds{1000}));

  // Status: XN-C peer received Handover Cancel.
  ASSERT_EQ(get_last_message().pdu.type().value, asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_cancel);

  // Procedure should have failed.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

/// If XNAP is stopped while a handover preparation procedure is awaiting the peer's response, the procedure must
/// fail gracefully rather than crash (transaction_sink.response() must not be called on a cancelled transaction).
TEST_F(xnap_handover_preparation_procedure_test, when_xnap_stopped_then_pending_handover_preparation_fails)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Create UE context.
  cu_cp_ue_index_t ue_index = create_ue();

  // Generate Security context for the UE.
  security::security_context sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
  xnap_handover_request      request  = generate_handover_request(ue_index, sec_ctxt);

  // Launch HO preparation procedure.
  async_task<xnap_handover_preparation_response>         t = xnap->handle_handover_request_required(request);
  lazy_task_launcher<xnap_handover_preparation_response> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Stop XNAP while the procedure is still awaiting the peer's response.
  async_task<void>         stop_task = xnap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);
  ASSERT_TRUE(stop_task.ready());

  // The pending procedure must have failed gracefully.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

/// Test successful handover preparation procedure.
TEST_F(xnap_handover_preparation_procedure_test, when_handover_request_ack_received_then_procedure_succeeds)
{
  // Run XN setup..
  run_xn_setup(xnap_peer_cfg);

  // Create UE context.
  cu_cp_ue_index_t ue_index = create_ue();

  // Generate Security context for the UE.
  security::security_context sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
  xnap_handover_request      request  = generate_handover_request(ue_index, sec_ctxt);

  // Action 1: Launch HO preparation procedure
  logger.info("Launch source XNAP handover preparation procedure");
  async_task<xnap_handover_preparation_response>         t = xnap->handle_handover_request_required(request);
  lazy_task_launcher<xnap_handover_preparation_response> t_launcher(t);

  // Status: XN-C peer received Handover Required.
  ASSERT_EQ(get_last_message().pdu.type().value, asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_request);

  ASSERT_FALSE(t.ready());

  // Inject Handover Request Acknowledge.
  xnap_message ho_request_ack = ::generate_handover_request_ack(local_xnap_ue_id_t::min, peer_xnap_ue_id_t::min);
  xnap->handle_message(ho_request_ack);

  // Procedure should have succeeded.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

///////////////////////////////////////////////////////////////////////////////
//                             Target CU-CP
///////////////////////////////////////////////////////////////////////////////

/// Test unsuccessful handover resource allocation procedure.
TEST_F(xnap_handover_preparation_procedure_test,
       when_handover_request_received_and_local_preparation_fails_then_handover_failure_is_sent)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Prepare handover to fail.
  set_handover_procedure_outcome(false);

  // Action 1: Inject Handover Request.
  xnap_message request = ::generate_handover_request(local_xnap_ue_id_t::min);
  xnap->handle_message(request);

  // Check Handover Preparation Failure.
  xnap_message rep = get_last_message();
  ASSERT_EQ(rep.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(rep.pdu.unsuccessful_outcome().value.type(),
            asn1::xnap::xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail);
}

/// Test successful handover resource allocation procedure.
TEST_F(xnap_handover_preparation_procedure_test,
       when_handover_request_received_and_local_preparation_succeeds_then_handover_request_ack_is_sent)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Prepare handover to succeed.
  set_handover_procedure_outcome(true);

  // Action 1: Inject Handover Request.
  xnap_message request = ::generate_handover_request(local_xnap_ue_id_t::min);
  xnap->handle_message(request);

  // Check Handover Request Ack.
  xnap_message rep = get_last_message();
  ASSERT_EQ(rep.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(rep.pdu.successful_outcome().value.type(),
            asn1::xnap::xnap_elem_procs_o::successful_outcome_c::types_opts::ho_request_ack);
}
