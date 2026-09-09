// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/xnap/procedures/xn_setup_procedure_asn1_helpers.h"
#include "xnap_test_helpers.h"
#include "xnap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::ocucp;

/// Fixture class for XNAP Setup tests.
class xn_setup_procedure_test : public xnap_test
{};

TEST_F(xn_setup_procedure_test, when_correct_setup_received_from_peer_setup_complete_is_sent)
{
  // Conect TX notifier to the XNAP instance, so that we can capture the response to the setup request.

  xnap_message xn_setup_req = generate_asn1_xn_setup_request(xnap_peer_cfg, {});
  xnap->handle_message(xn_setup_req);

  // Check XN setup response.
  xnap_message rep = get_last_message();
  ASSERT_EQ(rep.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(rep.pdu.successful_outcome().value.type(),
            asn1::xnap::xnap_elem_procs_o::successful_outcome_c::types_opts::xn_setup_resp);
}

/// Test the XN setup procedure timeout
TEST_F(xnap_test, when_xn_setup_procedure_times_out_then_setup_failure_is_returned)
{
  // Action 1: Launch XN setup procedure
  logger.info("Launch xn setup request procedure...");
  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Check XN setup request.
  xnap_message setup_req = get_last_message();
  ASSERT_EQ(setup_req.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(setup_req.pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request);

  // Status: Fail XN setup procedure (Xn-C peer doesn't respond).
  ASSERT_TRUE(this->tick(t, std::chrono::milliseconds(5000)));

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get());
}

TEST_F(xn_setup_procedure_test,
       when_xn_setup_failure_with_time_to_wait_received_then_setup_is_retried_after_time_to_wait)
{
  // Action 1: Launch XN setup procedure
  logger.info("Launch xn setup request procedure...");
  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Check XN setup request.
  xnap_message setup_req = get_last_message();
  ASSERT_EQ(setup_req.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(setup_req.pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request);

  // Action 2: Send XN setup failure with time to wait from peer.
  xnap_message setup_fail =
      generate_asn1_xn_setup_failure(xnap_cause_radio_network_t::unspecified, asn1::xnap::time_to_wait_e::v10s);
  xnap->handle_message(setup_fail);

  // Status: Xn-C peer does not receive new XN Setup Request until time-to-wait has ended.
  ASSERT_TRUE(this->tick(t, std::chrono::milliseconds(10000)));

  // Check XN setup request is sent again.
  setup_req = get_last_message();
  ASSERT_EQ(setup_req.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(setup_req.pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request);

  // Action 3: Send XN setup response from peer.
  xnap_message setup_resp = generate_asn1_xn_setup_response(xnap_peer_cfg, {});
  xnap->handle_message(setup_resp);

  // Check procedure completion.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
}

TEST_F(xn_setup_procedure_test, when_xn_setup_failure_without_time_to_wait_received_then_setup_fails)
{
  // Action 1: Launch XN setup procedure
  logger.info("Launch xn setup request procedure...");
  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Check XN setup request.
  xnap_message setup_req = get_last_message();
  ASSERT_EQ(setup_req.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(setup_req.pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request);

  // Action 2: Send XN setup failure without time to wait from peer.
  xnap_message setup_fail = generate_asn1_xn_setup_failure(xnap_cause_radio_network_t::unspecified);
  xnap->handle_message(setup_fail);

  // Check procedure completion.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get());
}

TEST_F(xn_setup_procedure_test, when_xn_setup_request_required_then_setup_is_sent_to_peer)
{
  // Action 1: Launch XN setup procedure
  logger.info("Launch xn setup request procedure...");
  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Check XN setup request.
  xnap_message setup_req = get_last_message();
  ASSERT_EQ(setup_req.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(setup_req.pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request);

  // Action 2: Send XN setup response from peer.
  xnap_message setup_resp = generate_asn1_xn_setup_response(xnap_peer_cfg, {});
  xnap->handle_message(setup_resp);

  // Check procedure completion.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
}

TEST_F(xn_setup_procedure_test, when_xn_setup_request_is_sent_then_it_carries_the_cells_this_node_serves)
{
  const cu_cp_served_cell_info served_cell = generate_served_cell_info(
      2, nr_cell_global_id_t{local_plmn, nr_cell_identity::create(local_gnb_id, 1).value()}, local_tac);
  cu_cp_notifier.set_served_cells({served_cell});

  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);

  const xnap_message setup_req = get_last_message();
  const auto&        asn1_ies  = setup_req.pdu.init_msg().value.xn_setup_request();
  ASSERT_TRUE(asn1_ies->list_of_served_cells_nr_present);
  ASSERT_EQ(asn1_ies->list_of_served_cells_nr.size(), 1);

  const auto& asn1_cell_info = asn1_ies->list_of_served_cells_nr[0].served_cell_info_nr;
  EXPECT_EQ(asn1_cell_info.nr_pci, served_cell.nr_pci);
  EXPECT_EQ(asn1_to_cgi(asn1_cell_info.cell_id), served_cell.nr_cgi);
  EXPECT_EQ(asn1_cell_info.tac.to_number(), local_tac);
}

TEST_F(xn_setup_procedure_test, when_xn_setup_response_is_sent_then_it_carries_the_cells_this_node_serves)
{
  const cu_cp_served_cell_info served_cell = generate_served_cell_info(
      2, nr_cell_global_id_t{local_plmn, nr_cell_identity::create(local_gnb_id, 1).value()}, local_tac);
  cu_cp_notifier.set_served_cells({served_cell});

  xnap->handle_message(generate_asn1_xn_setup_request(xnap_peer_cfg, {}));

  const xnap_message setup_resp = get_last_message();
  const auto&        asn1_ies   = setup_resp.pdu.successful_outcome().value.xn_setup_resp();
  ASSERT_TRUE(asn1_ies->list_of_served_cells_nr_present);
  ASSERT_EQ(asn1_ies->list_of_served_cells_nr.size(), 1);

  const auto& asn1_cell_info = asn1_ies->list_of_served_cells_nr[0].served_cell_info_nr;
  EXPECT_EQ(asn1_cell_info.nr_pci, served_cell.nr_pci);
  EXPECT_EQ(asn1_to_cgi(asn1_cell_info.cell_id), served_cell.nr_cgi);
}

TEST_F(xn_setup_procedure_test, when_peer_advertises_served_cells_then_peer_is_found_by_served_cell_pci)
{
  const pci_t               served_pci = 42;
  const nr_cell_global_id_t served_cgi{peer_plmn, nr_cell_identity::create(0x19b0).value()};

  // Before the XN setup procedure completes, no served cell of the peer is known.
  ASSERT_FALSE(xnap->has_peer_pci(served_pci)) << "Served cells must not be known before the XN setup completed";

  // Action: Run the XN setup procedure with a peer advertising a served NR cell.
  async_task<bool>         t = xnap->handle_xn_setup_request_required();
  lazy_task_launcher<bool> t_launcher(t);
  xnap_message setup_resp = generate_xn_setup_response_with_served_cell(xnap_peer_cfg, served_pci, served_cgi);
  xnap->handle_message(setup_resp);
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());

  ASSERT_TRUE(xnap->has_peer_pci(served_pci)) << "Peer was not found by the PCI of its served cell";
  ASSERT_FALSE(xnap->has_peer_pci(served_pci + 1)) << "Unexpected match for a PCI the peer does not serve";
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
