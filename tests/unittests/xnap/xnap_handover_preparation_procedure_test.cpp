// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/rrc/rrc_ue_test_helpers.h"
#include "tests/unittests/xnap/xnap_test_messages.h"
#include "xnap_test_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/inter_cu_handover_messages.h"
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
    request.ue_context_info_ho_request.amf_ue_id        = to_underlying(amf_ue_id_t::min);
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

  /// \brief Prepares two CHO candidates at this node under one Source NG-RAN node UE XnAP ID, and returns the Target
  /// NG-RAN node UE XnAP ID each of them was given, in preparation order.
  std::vector<uint64_t> prepare_two_target_candidates(local_xnap_ue_id_t source_ue_id)
  {
    run_xn_setup(xnap_peer_cfg);
    set_handover_procedure_outcome(true);
    pop_sent_messages();

    xnap->handle_message(::generate_handover_request(source_ue_id));
    xnap->handle_message(::generate_handover_request(source_ue_id));

    std::vector<uint64_t>     local_ids;
    std::vector<xnap_message> acks = pop_sent_messages();
    EXPECT_EQ(acks.size(), 2);
    for (const xnap_message& ack : acks) {
      const auto& ho_ack = ack.pdu.successful_outcome().value.ho_request_ack();
      EXPECT_EQ(ho_ack->source_ng_ra_nnode_ue_xn_ap_id, to_underlying(source_ue_id));
      local_ids.push_back(ho_ack->target_ng_ra_nnode_ue_xn_ap_id);
    }
    EXPECT_NE(local_ids[0], local_ids[1]) << "each candidate must get its own Target NG-RAN node UE XnAP ID";
    return local_ids;
  }
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

  // Status: Xn-C peer received Handover Required.
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

  // Status: Xn-C peer received Handover Required.
  ASSERT_EQ(get_last_message().pdu.type().value, asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_request);

  ASSERT_FALSE(t.ready());

  // Status: Fail Handover Preparation procedure (Xn-C peer doesn't respond).
  ASSERT_TRUE(this->tick(t, std::chrono::milliseconds{1000}));

  // Status: Xn-C peer received Handover Cancel.
  ASSERT_EQ(get_last_message().pdu.type().value, asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_cancel);

  // Procedure should have failed.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

/// If XNAP is stopped while a handover preparation procedure is awaiting the peer's response, the procedure must
/// fail gracefully (transaction_sink.response() must not be called on a cancelled transaction).
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

  // Status: Xn-C peer received Handover Required.
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

/// Test that the procedure survives the removal of the XNAP UE context while it awaits the RRC Handover Command. That
/// suspension is not a XNAP transaction, so stopping XNAP does not resume the procedure and the UE context can be
/// destroyed underneath it.
TEST_F(xnap_handover_preparation_procedure_test,
       when_ue_context_is_removed_while_awaiting_rrc_command_then_procedure_finishes)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Create UE context.
  cu_cp_ue_index_t ue_index = create_ue();

  // Generate Security context for the UE.
  security::security_context sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
  xnap_handover_request      request  = generate_handover_request(ue_index, sec_ctxt);

  // Suspend the procedure while it awaits the RRC Handover Command.
  cu_cp_notifier.defer_rrc_handover_command();

  logger.info("Launch source XNAP handover preparation procedure");
  async_task<xnap_handover_preparation_response>         t = xnap->handle_handover_request_required(request);
  lazy_task_launcher<xnap_handover_preparation_response> t_launcher(t);

  // Inject Handover Request Acknowledge, so the procedure proceeds to await the RRC Handover Command.
  xnap_message ho_request_ack = ::generate_handover_request_ack(local_xnap_ue_id_t::min, peer_xnap_ue_id_t::min);
  xnap->handle_message(ho_request_ack);

  ASSERT_FALSE(t.ready());

  // Remove the XNAP UE context while the procedure is suspended.
  xnap->get_xnap_ue_context_removal_handler().remove_ue_context(ue_index);

  // Let the procedure resume and report its outcome.
  cu_cp_notifier.complete_rrc_handover_command();

  ASSERT_TRUE(t.ready());
}

/// Test that the Handover Request reports this source's own DRB-to-QoS-flow mapping via the Data Forwarding and
/// Offloading Info from source NG-RAN node IE (TS 38.423 Section 9.2.1.17), so the target can prefer the same DRB
/// numbering during admission instead of allocating DRB IDs blind to the source's own configuration.
TEST_F(xnap_handover_preparation_procedure_test,
       when_handover_request_sent_then_rrc_handover_preparation_info_is_forwarded)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Create UE context.
  cu_cp_ue_index_t ue_index = create_ue();

  // Generate Security context for the UE.
  security::security_context sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
  xnap_handover_request      request  = generate_handover_request(ue_index, sec_ctxt);

  // The RRC layer embeds the source's full current radio bearer configuration (including its DRB-to-QoS-flow
  // mapping) inside the HandoverPreparationInformation's AS-Config (TS 38.331 Section 11.2.3). XNAP only needs to
  // forward this RRC container transparently.
  byte_buffer rrc_handover_preparation_info                               = make_byte_buffer("deadbeef").value();
  request.ue_context_info_ho_request.rrc_handover_preparation_information = rrc_handover_preparation_info.copy();

  // Action: Launch HO preparation procedure.
  async_task<xnap_handover_preparation_response>         t = xnap->handle_handover_request_required(request);
  lazy_task_launcher<xnap_handover_preparation_response> t_launcher(t);

  xnap_message sent_msg = get_last_message();
  ASSERT_EQ(sent_msg.pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_request);
  const auto& ho_request = sent_msg.pdu.init_msg().value.ho_request();

  EXPECT_EQ(ho_request->ue_context_info_ho_request.rrc_context, rrc_handover_preparation_info);
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

/// Test that stopping XNAP waits for a UE procedure that is suspended on a CU-CP notifier. Cancelling the XNAP
/// transactions does not resume such a procedure, and the caller removes the XNAP instance as soon as stop() returns.
TEST_F(xnap_handover_preparation_procedure_test, when_ue_procedure_is_in_flight_then_stop_awaits_its_completion)
{
  // Run XN setup.
  run_xn_setup(xnap_peer_cfg);

  // Prepare handover to succeed.
  set_handover_procedure_outcome(true);

  // Suspend the target handover preparation procedure inside the CU-CP.
  cu_cp_notifier.defer_handover_request();

  // Inject Handover Request.
  xnap_message request = ::generate_handover_request(local_xnap_ue_id_t::min);
  xnap->handle_message(request);

  // Stop XNAP while the procedure is suspended. The stop task must not complete yet.
  async_task<void>         stop_task = xnap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);
  ASSERT_FALSE(stop_task.ready());

  // Destroy XNAP as soon as it reports being stopped, the way xnap_repository::remove_xnap does. The procedure is
  // still in flight here, so this must not happen yet.
  if (stop_task.ready()) {
    xnap.reset();
  }

  // Let the procedure resume and run to completion.
  cu_cp_notifier.complete_handover_request();

  // Only now may XNAP be considered stopped.
  ASSERT_TRUE(stop_task.ready());
}

/// TS 38.423 Section 9.1.1.4: the SN STATUS TRANSFER is addressed by the Target NG-RAN node UE XnAP ID, which this node
/// allocated. Parallel CHO preparations from one source share the Source NG-RAN node UE XnAP ID, so routing on that ID
/// alone hands one candidate's status to another and leaves the addressed candidate waiting for a message it received.
TEST_F(xnap_handover_preparation_procedure_test,
       when_two_target_contexts_share_a_source_ue_id_then_sn_status_transfer_reaches_the_addressed_one)
{
  const local_xnap_ue_id_t    source_ue_id = local_xnap_ue_id_t::min;
  const std::vector<uint64_t> local_ids    = prepare_two_target_candidates(source_ue_id);
  ASSERT_EQ(local_ids.size(), 2);

  const cu_cp_ue_index_t ue_index_b = uint_to_ue_index(to_underlying(cu_cp_ue_index_t::min) + 1);
  ASSERT_TRUE(xnap->has_ue_context(cu_cp_ue_index_t::min));
  ASSERT_TRUE(xnap->has_ue_context(ue_index_b));

  async_task<expected<cu_cp_status_transfer>>         t_b = xnap->handle_sn_status_transfer_expected(ue_index_b);
  lazy_task_launcher<expected<cu_cp_status_transfer>> launcher_b(t_b);
  ASSERT_FALSE(t_b.ready());

  // The UE accessed the second candidate, so the source addresses that context.
  xnap->handle_message(::generate_sn_status_transfer(source_ue_id, uint_to_peer_xnap_ue_id(local_ids[1])));

  ASSERT_TRUE(t_b.ready()) << "SN Status Transfer was routed to another candidate's context";
  ASSERT_TRUE(t_b.get().has_value());
}

/// TS 38.423 Section 8.2.3.2: a HANDOVER CANCEL cancels the handover on the signalling connection identified by the
/// Source NG-RAN node UE XnAP ID and, if included, the Target NG-RAN node UE XnAP ID. Parallel CHO preparations share
/// the source ID, so only the target ID picks out one of them; cancelling one must leave its sibling prepared.
TEST_F(xnap_handover_preparation_procedure_test,
       when_two_target_contexts_share_a_source_ue_id_then_handover_cancel_releases_only_the_named_one)
{
  const local_xnap_ue_id_t    source_ue_id = local_xnap_ue_id_t::min;
  const std::vector<uint64_t> local_ids    = prepare_two_target_candidates(source_ue_id);
  ASSERT_EQ(local_ids.size(), 2);

  const cu_cp_ue_index_t ue_index_a = cu_cp_ue_index_t::min;
  const cu_cp_ue_index_t ue_index_b = uint_to_ue_index(to_underlying(cu_cp_ue_index_t::min) + 1);
  ASSERT_TRUE(xnap->has_ue_context(ue_index_a));
  ASSERT_TRUE(xnap->has_ue_context(ue_index_b));

  // The source cancels the second candidate only. Both candidates were prepared for the cell that
  // generate_handover_request() names, so the Target NG-RAN node UE XnAP ID is what tells them apart.
  const nr_cell_global_id_t prepared_cell{plmn_identity::test_value(), nr_cell_identity::create({411, 22}, 0).value()};
  xnap->handle_message(::generate_handover_cancel(source_ue_id, uint_to_peer_xnap_ue_id(local_ids[1]), prepared_cell));

  ASSERT_TRUE(xnap->has_ue_context(ue_index_a)) << "the sibling candidate was released by another candidate's cancel";
  ASSERT_FALSE(xnap->has_ue_context(ue_index_b)) << "the named candidate was not released";
}

///////////////////////////////////////////////////////////////////////////////
//                    Source CU-CP - conditional handover
///////////////////////////////////////////////////////////////////////////////

/// Fixture for CHO preparations towards two candidate cells served by the same XN-C peer.
class xnap_cho_preparation_test : public xnap_handover_preparation_procedure_test
{
protected:
  void SetUp() override
  {
    run_xn_setup(xnap_peer_cfg);
    ue_index = create_ue();
    sec_ctxt = generate_security_context(ue_mng.find_ue(ue_index)->get_security_manager());
    // Discard the XN setup exchange, so that each test only sees the messages it triggers.
    pop_sent_messages();
  }

  /// \brief Launches a CHO preparation towards \c cell and returns the pending task.
  xnap_handover_request make_cho_request(const nr_cell_global_id_t& cell)
  {
    xnap_handover_request request   = generate_handover_request(ue_index, sec_ctxt);
    request.is_conditional_handover = true;
    request.nr_cgi                  = cell;
    return request;
  }

  const nr_cell_global_id_t cell_a{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 1).value()};
  const nr_cell_global_id_t cell_b{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 2).value()};

  // TS 38.423 Section 8.2.1.1: parallel CHO preparations share one Source NG-RAN node UE XnAP ID.
  const local_xnap_ue_id_t local_id = local_xnap_ue_id_t::min;
  const peer_xnap_ue_id_t  peer_a   = peer_xnap_ue_id_t::min;
  const peer_xnap_ue_id_t  peer_b   = uint_to_peer_xnap_ue_id(to_underlying(peer_xnap_ue_id_t::min) + 1);

  cu_cp_ue_index_t           ue_index = cu_cp_ue_index_t::invalid;
  security::security_context sec_ctxt;
};

/// Two CHO candidates at the same peer share one Source NG-RAN node UE XnAP ID, and each acknowledgement resolves
/// only the preparation for the cell it names.
TEST_F(xnap_cho_preparation_test, when_two_cho_candidates_prepared_then_each_ack_resolves_its_own_cell)
{
  async_task<xnap_handover_preparation_response> t_a = xnap->handle_handover_request_required(make_cho_request(cell_a));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_a(t_a);
  async_task<xnap_handover_preparation_response> t_b = xnap->handle_handover_request_required(make_cho_request(cell_b));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_b(t_b);

  // Both Handover Requests carry the same source XNAP UE ID.
  std::vector<xnap_message> requests = pop_sent_messages();
  ASSERT_EQ(requests.size(), 2);
  ASSERT_EQ(requests[0].pdu.init_msg().value.ho_request()->source_ng_ra_nnode_ue_xn_ap_id, to_underlying(local_id));
  ASSERT_EQ(requests[1].pdu.init_msg().value.ho_request()->source_ng_ra_nnode_ue_xn_ap_id, to_underlying(local_id));

  ASSERT_FALSE(t_a.ready());
  ASSERT_FALSE(t_b.ready());

  // The acknowledgement for cell A resolves only the preparation for cell A.
  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_a, cell_a));
  ASSERT_TRUE(t_a.ready());
  ASSERT_TRUE(t_a.get().success);
  ASSERT_FALSE(t_b.ready());

  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_b, cell_b));
  ASSERT_TRUE(t_b.ready());
  ASSERT_TRUE(t_b.get().success);
}

/// A Handover Preparation Failure naming one candidate cell must fail only that preparation.
TEST_F(xnap_cho_preparation_test, when_one_cho_candidate_fails_then_the_other_stays_pending)
{
  async_task<xnap_handover_preparation_response> t_a = xnap->handle_handover_request_required(make_cho_request(cell_a));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_a(t_a);
  async_task<xnap_handover_preparation_response> t_b = xnap->handle_handover_request_required(make_cho_request(cell_b));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_b(t_b);
  pop_sent_messages();

  xnap->handle_message(::generate_cho_handover_preparation_failure(local_id, cell_a));
  ASSERT_TRUE(t_a.ready());
  ASSERT_FALSE(t_a.get().success);
  ASSERT_FALSE(t_b.ready());

  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_b, cell_b));
  ASSERT_TRUE(t_b.ready());
  ASSERT_TRUE(t_b.get().success);
}

/// Cancelling a non-winning candidate sends a HandoverCancel naming that candidate's cell and target XNAP UE ID,
/// and keeps the UE context alive for the remaining candidate.
TEST_F(xnap_cho_preparation_test, when_non_winner_cancelled_then_handover_cancel_names_that_candidate)
{
  async_task<xnap_handover_preparation_response> t_a = xnap->handle_handover_request_required(make_cho_request(cell_a));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_a(t_a);
  async_task<xnap_handover_preparation_response> t_b = xnap->handle_handover_request_required(make_cho_request(cell_b));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_b(t_b);

  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_a, cell_a));
  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_b, cell_b));
  ASSERT_TRUE(t_a.ready() and t_a.get().success);
  ASSERT_TRUE(t_b.ready() and t_b.get().success);
  pop_sent_messages();

  // Cancel the candidate at cell B; cell A won.
  xnap->handle_cho_cancel_required(ue_index, cell_b);

  std::vector<xnap_message> cancels = pop_sent_messages();
  ASSERT_EQ(cancels.size(), 1);
  ASSERT_EQ(cancels[0].pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_cancel);
  const asn1::xnap::ho_cancel_s& cancel = cancels[0].pdu.init_msg().value.ho_cancel();
  ASSERT_EQ(cancel->source_ng_ra_nnode_ue_xn_ap_id, to_underlying(local_id));
  ASSERT_TRUE(cancel->target_ng_ra_nnode_ue_xn_ap_id_present);
  ASSERT_EQ(cancel->target_ng_ra_nnode_ue_xn_ap_id, to_underlying(peer_b));
  ASSERT_TRUE(cancel->target_cells_to_cancel_present);
  ASSERT_EQ(cancel->target_cells_to_cancel.size(), 1);
  ASSERT_EQ(cancel->target_cells_to_cancel[0].target_cell.nr().nr_ci.to_number(), cell_b.nci.value());

  // The winning candidate keeps the UE context alive.
  ASSERT_TRUE(xnap->has_ue_context(ue_index));

  // Cancelling the last candidate releases it.
  xnap->handle_cho_cancel_required(ue_index, cell_a);
  ASSERT_FALSE(xnap->has_ue_context(ue_index));
}

/// When one CHO candidate times out, the HandoverCancel that follows must be scoped to that candidate alone
/// (TS 38.423 Section 8.2.3.2). A cancel that omits Target Cells to Cancel releases the whole UE-associated
/// signalling connection, taking the sibling candidates with it.
TEST_F(xnap_cho_preparation_test, when_one_cho_candidate_times_out_then_the_cancel_names_only_that_cell)
{
  async_task<xnap_handover_preparation_response> t_a = xnap->handle_handover_request_required(make_cho_request(cell_a));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_a(t_a);
  async_task<xnap_handover_preparation_response> t_b = xnap->handle_handover_request_required(make_cho_request(cell_b));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_b(t_b);

  // Cell B is prepared; cell A never gets an answer.
  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_b, cell_b));
  ASSERT_TRUE(t_b.ready() and t_b.get().success);
  pop_sent_messages();

  ASSERT_TRUE(this->tick(t_a, std::chrono::milliseconds{1000}));
  ASSERT_TRUE(t_a.ready());
  ASSERT_FALSE(t_a.get().success);

  std::vector<xnap_message> sent = pop_sent_messages();
  ASSERT_EQ(sent.size(), 1);
  ASSERT_EQ(sent[0].pdu.init_msg().value.type().value,
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ho_cancel);
  const asn1::xnap::ho_cancel_s& cancel = sent[0].pdu.init_msg().value.ho_cancel();
  ASSERT_TRUE(cancel->target_cells_to_cancel_present) << "the cancel must be scoped to the candidate that timed out";
  ASSERT_EQ(cancel->target_cells_to_cancel.size(), 1);
  ASSERT_EQ(cancel->target_cells_to_cancel[0].target_cell.nr().nr_ci.to_number(), cell_a.nci.value());
}

/// A CHO preparation that fails before it is sent must drop its per-cell event source, so that the same candidate
/// cell can be prepared again.
TEST_F(xnap_cho_preparation_test, when_cho_preparation_fails_early_then_the_same_cell_can_be_prepared_again)
{
  xnap_handover_request bad_request = make_cho_request(cell_a);
  bad_request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list.clear();

  async_task<xnap_handover_preparation_response>         t_bad = xnap->handle_handover_request_required(bad_request);
  lazy_task_launcher<xnap_handover_preparation_response> launcher_bad(t_bad);
  ASSERT_TRUE(t_bad.ready());
  ASSERT_FALSE(t_bad.get().success);

  // Retrying the same candidate cell must be accepted, not rejected as already in flight.
  async_task<xnap_handover_preparation_response> t_retry =
      xnap->handle_handover_request_required(make_cho_request(cell_a));
  lazy_task_launcher<xnap_handover_preparation_response> launcher_retry(t_retry);
  ASSERT_FALSE(t_retry.ready()) << "the retry was refused instead of being sent";

  xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_a, cell_a));
  ASSERT_TRUE(t_retry.ready());
  ASSERT_TRUE(t_retry.get().success);
}

/// TS 38.331 allows up to eight conditional reconfigurations, so a UE can hold eight CHO candidates at the same
/// peer. All of them share one Source NG-RAN node UE XnAP ID and must each resolve on their own cell.
TEST_F(xnap_cho_preparation_test, when_eight_cho_candidates_prepared_then_all_resolve_independently)
{
  constexpr unsigned nof_candidates = 8;

  std::vector<nr_cell_global_id_t>                                                     cells;
  std::vector<async_task<xnap_handover_preparation_response>>                          tasks;
  std::vector<std::unique_ptr<lazy_task_launcher<xnap_handover_preparation_response>>> launchers;
  tasks.reserve(nof_candidates);

  for (unsigned i = 0; i != nof_candidates; ++i) {
    cells.push_back(nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, i + 1).value()});
    tasks.push_back(xnap->handle_handover_request_required(make_cho_request(cells.back())));
    launchers.push_back(std::make_unique<lazy_task_launcher<xnap_handover_preparation_response>>(tasks.back()));
  }

  // All eight Handover Requests went out under one Source NG-RAN node UE XnAP ID.
  std::vector<xnap_message> requests = pop_sent_messages();
  ASSERT_EQ(requests.size(), nof_candidates);
  for (const xnap_message& request : requests) {
    ASSERT_EQ(request.pdu.init_msg().value.ho_request()->source_ng_ra_nnode_ue_xn_ap_id, to_underlying(local_id));
  }
  for (const auto& task : tasks) {
    ASSERT_FALSE(task.ready());
  }

  // Acknowledge them out of order; each ack must resolve only its own candidate.
  const std::vector<unsigned> ack_order = {3, 0, 7, 5, 1, 6, 2, 4};
  for (unsigned acked = 0; acked != ack_order.size(); ++acked) {
    const unsigned          cand    = ack_order[acked];
    const peer_xnap_ue_id_t peer_id = uint_to_peer_xnap_ue_id(to_underlying(peer_xnap_ue_id_t::min) + cand);

    xnap->handle_message(::generate_cho_handover_request_ack(local_id, peer_id, cells[cand]));

    ASSERT_TRUE(tasks[cand].ready()) << "candidate " << cand << " did not resolve on its own ack";
    ASSERT_TRUE(tasks[cand].get().success);

    // Every candidate not acknowledged yet must still be pending.
    for (unsigned later = acked + 1; later != ack_order.size(); ++later) {
      ASSERT_FALSE(tasks[ack_order[later]].ready())
          << "candidate " << ack_order[later] << " resolved on another candidate's ack";
    }
  }
}
