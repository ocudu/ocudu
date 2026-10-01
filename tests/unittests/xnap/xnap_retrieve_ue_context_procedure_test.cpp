// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/xnap/xnap_test_message_validators.h"
#include "tests/unittests/xnap/xnap_test_messages.h"
#include "xnap_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/xnap/xnap_ue_context_retrieval.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture class for XNAP UE context retrieval procedure tests (TS 38.423 section 8.2.4).
class xnap_retrieve_ue_context_procedure_test : public xnap_test
{
public:
  /// PCI and cell identity of the cell the peer serves, i.e. the cell the UE came from.
  static constexpr pci_t peer_pci = 42;
  nr_cell_identity       peer_nci = nr_cell_identity::create(0x19b0).value();
  /// Cell identity of the local cell the UE is reestablishing on.
  nr_cell_identity local_nci = nr_cell_identity::create(0x19b1).value();

  /// Runs XN setup so that the peer context, including its served cells, is known.
  void run_xn_setup_with_served_cell()
  {
    async_task<bool>         t = xnap->handle_xn_setup_request_required();
    lazy_task_launcher<bool> t_launcher(t);
    xnap->handle_message(
        generate_xn_setup_response_with_served_cell(xnap_peer_cfg, peer_pci, nr_cell_global_id_t{peer_plmn, peer_nci}));
    ASSERT_TRUE(t.get());
  }

  static xnap_retrieve_ue_context_request generate_request(cu_cp_ue_index_t ue_index, nr_cell_identity target_nci)
  {
    xnap_retrieve_ue_context_request request;
    request.ue_index      = ue_index;
    request.ue_context_id = xnap_ue_context_id_for_rrc_reest{.c_rnti = rnti_t::MIN_CRNTI, .fail_cell_pci = peer_pci};
    request.mac_i         = {0xab, 0xcd};
    request.target_nci    = target_nci;
    return request;
  }
};

//----------------------------------------------------------------------------------//
// New NG-RAN node side: this node retrieves the UE context from the peer           //
//----------------------------------------------------------------------------------//

TEST_F(xnap_retrieve_ue_context_procedure_test, when_retrieval_is_required_then_request_is_sent_to_peer)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  const cu_cp_ue_index_t ue_index = create_ue();
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);

  async_task<xnap_retrieve_ue_context_response> t =
      xnap->handle_retrieve_ue_context_required(generate_request(ue_index, local_nci));
  lazy_task_launcher<xnap_retrieve_ue_context_response> t_launcher(t);

  ASSERT_FALSE(t.ready());

  const xnap_message msg = get_last_message();
  ASSERT_TRUE(test_helpers::is_valid_retrieve_ue_context_request(msg));
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_response_is_received_then_ue_context_is_returned)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  const cu_cp_ue_index_t ue_index = create_ue();
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);

  async_task<xnap_retrieve_ue_context_response> t =
      xnap->handle_retrieve_ue_context_required(generate_request(ue_index, local_nci));
  lazy_task_launcher<xnap_retrieve_ue_context_response> t_launcher(t);

  const local_xnap_ue_id_t local_xnap_ue_id = uint_to_local_xnap_ue_id(
      get_last_message().pdu.init_msg().value.retrieve_ue_context_request()->new_ng_ra_nnode_ue_xn_ap_id);

  xnap->handle_message(generate_retrieve_ue_context_response(local_xnap_ue_id, uint_to_peer_xnap_ue_id(7)));

  ASSERT_TRUE(t.ready());
  const xnap_retrieve_ue_context_response response = t.get();
  ASSERT_TRUE(response.success);
  ASSERT_EQ(response.peer_xnap_ue_id, uint_to_peer_xnap_ue_id(7))
      << "The XNAP UE ID the old NG-RAN node allocated was not learned from the response";
  ASSERT_EQ(response.ue_context_info.pdu_session_res_to_be_setup_list.size(), 1);
  ASSERT_FALSE(response.ue_context_info.rrc_context.empty());
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_failure_is_received_then_retrieval_fails)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  const cu_cp_ue_index_t ue_index = create_ue();
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);

  async_task<xnap_retrieve_ue_context_response> t =
      xnap->handle_retrieve_ue_context_required(generate_request(ue_index, local_nci));
  lazy_task_launcher<xnap_retrieve_ue_context_response> t_launcher(t);

  const local_xnap_ue_id_t local_xnap_ue_id = uint_to_local_xnap_ue_id(
      get_last_message().pdu.init_msg().value.retrieve_ue_context_request()->new_ng_ra_nnode_ue_xn_ap_id);

  xnap->handle_message(generate_retrieve_ue_context_failure(local_xnap_ue_id));

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_peer_does_not_respond_then_retrieval_times_out)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  const cu_cp_ue_index_t ue_index = create_ue();
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);

  async_task<xnap_retrieve_ue_context_response> t =
      xnap->handle_retrieve_ue_context_required(generate_request(ue_index, local_nci));
  lazy_task_launcher<xnap_retrieve_ue_context_response> t_launcher(t);

  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(this->tick(t, std::chrono::milliseconds(1000)));

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_xnap_stopped_then_pending_retrieval_fails)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  const cu_cp_ue_index_t ue_index = create_ue();
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);

  async_task<xnap_retrieve_ue_context_response> t =
      xnap->handle_retrieve_ue_context_required(generate_request(ue_index, local_nci));
  lazy_task_launcher<xnap_retrieve_ue_context_response> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Stopping XNAP must cancel the pending transaction, otherwise the procedure would never complete.
  async_task<void>         stop_task = xnap->stop();
  lazy_task_launcher<void> stop_launcher(stop_task);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

//----------------------------------------------------------------------------------//
// Old NG-RAN node side: the peer retrieves a UE context held by this node          //
//----------------------------------------------------------------------------------//

TEST_F(xnap_retrieve_ue_context_procedure_test, when_request_for_unknown_ue_context_id_received_then_failure_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  ASSERT_NO_FATAL_FAILURE(run_xn_setup_with_served_cell());

  cu_cp_notifier.ue_context_id_lookup_result = cu_cp_ue_index_t::invalid;

  xnap->handle_message(generate_retrieve_ue_context_request(uint_to_peer_xnap_ue_id(7), peer_pci, local_nci));

  const xnap_message msg = get_last_message();
  ASSERT_TRUE(test_helpers::is_valid_retrieve_ue_context_failure(msg));
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.retrieve_ue_context_fail()->new_ng_ra_nnode_ue_xn_ap_id, 7U);
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_request_for_unadvertised_target_cell_received_then_failure_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  ASSERT_NO_FATAL_FAILURE(run_xn_setup_with_served_cell());

  const cu_cp_ue_index_t ue_index            = create_ue();
  cu_cp_notifier.ue_context_id_lookup_result = ue_index;
  // The CU-CP would accept the retrieval, so a Failure can only come from the target cell not being resolvable.
  cu_cp_notifier.retrieve_ue_context_response.success = true;

  // Deriving KgNB* takes the target cell, so a cell the peer never advertised is rejected.
  const nr_cell_identity unknown_nci = nr_cell_identity::create(0x19bf).value();
  xnap->handle_message(generate_retrieve_ue_context_request(uint_to_peer_xnap_ue_id(7), peer_pci, unknown_nci));

  ASSERT_TRUE(test_helpers::is_valid_retrieve_ue_context_failure(get_last_message()));
  ASSERT_FALSE(cu_cp_notifier.last_retrieve_ue_context_request.has_value())
      << "The CU-CP must not be asked for a UE context that cannot be keyed to the target cell";
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_cu_cp_accepts_the_retrieval_then_response_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  ASSERT_NO_FATAL_FAILURE(run_xn_setup_with_served_cell());

  const cu_cp_ue_index_t ue_index                                         = create_ue();
  cu_cp_notifier.ue_context_id_lookup_result                              = ue_index;
  cu_cp_notifier.retrieve_ue_context_response.success                     = true;
  cu_cp_notifier.retrieve_ue_context_response.ue_context_info.rrc_context = make_byte_buffer("deadbeef").value();

  xnap->handle_message(generate_retrieve_ue_context_request(uint_to_peer_xnap_ue_id(7), peer_pci, peer_nci));

  ASSERT_TRUE(cu_cp_notifier.last_retrieve_ue_context_request.has_value());
  ASSERT_EQ(cu_cp_notifier.last_retrieve_ue_context_request->ue_index, ue_index);
  ASSERT_TRUE(cu_cp_notifier.last_retrieve_ue_context_request->target_cell.has_value())
      << "The target cell must be resolved from the served cell list of the peer";
  ASSERT_EQ(cu_cp_notifier.last_retrieve_ue_context_request->target_cell->nr_pci, peer_pci);

  const xnap_message msg = get_last_message();
  ASSERT_TRUE(test_helpers::is_valid_retrieve_ue_context_response(msg));
  ASSERT_EQ(msg.pdu.successful_outcome().value.retrieve_ue_context_resp()->new_ng_ra_nnode_ue_xn_ap_id, 7U);
}

TEST_F(xnap_retrieve_ue_context_procedure_test, when_cu_cp_rejects_the_retrieval_then_failure_is_sent)
{
  OCUDU_TEST_REQUIREMENTS("CU-GEN-3");

  ASSERT_NO_FATAL_FAILURE(run_xn_setup_with_served_cell());

  const cu_cp_ue_index_t ue_index                     = create_ue();
  cu_cp_notifier.ue_context_id_lookup_result          = ue_index;
  cu_cp_notifier.retrieve_ue_context_response.success = false;
  cu_cp_notifier.retrieve_ue_context_response.cause   = xnap_cause_radio_network_t::unspecified;

  xnap->handle_message(generate_retrieve_ue_context_request(uint_to_peer_xnap_ue_id(7), peer_pci, peer_nci));

  ASSERT_TRUE(test_helpers::is_valid_retrieve_ue_context_failure(get_last_message()));
}
