// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/routines/mobility/mobility_helpers.h"
#include "lib/cu_cp/xnap_repository.h"
#include "tests/unittests/cu_cp/cu_cp_test_environment.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/xnap/xnap_test_helpers.h"
#include "tests/unittests/xnap/xnap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_xnap_repository_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_xnap_repository_test() :
    cu_cp_test_environment({/* max nof cu-ups */ 8,
                            /* max nof dus */ 8,
                            /* max nof ues */ 8192,
                            /* max nof drbs per ue */ 8,
                            /* amf config */ {{default_supported_tracking_area}},
                            /* trigger ho from measurements */ true,
                            /* enable rrc inactive */ false,
                            /* enable xnc peer */ true}),
    ue_cfg([this]() {
      auto cfg = get_cu_cp_cfg();
      return ue_manager_config{.gnb_id              = cfg.node.gnb_id,
                               .max_nof_ues         = cfg.admission.max_nof_ues,
                               .drb_config          = cfg.bearers.drb_config,
                               .max_nof_drbs_per_ue = cfg.admission.max_nof_drbs_per_ue,
                               .int_algo_pref_list  = cfg.security.int_algo_pref_list,
                               .enc_algo_pref_list  = cfg.security.enc_algo_pref_list,
                               .enable_rrc_metrics  = cfg.metrics.layers_cfg.enable_rrc_metrics,
                               .ue                  = cfg.ue};
    }()),
    ue_dependencies([this]() {
      return ue_manager_dependencies{
          .timers = timers, .cu_cp_executor = ctrl_worker, .logger = ocudulog::fetch_basic_logger("CU-CP")};
    }()),
    ue_mng(ue_cfg, ue_dependencies),
    // Drive the XNAPs from the fixture's own manual worker, so that procedures run inline and can be stepped
    // deterministically from the test.
    local_cu_cp_cfg([this]() {
      cu_cp_configuration cfg     = get_cu_cp_cfg();
      cfg.services.timers         = &timers;
      cfg.services.cu_cp_executor = &ctrl_worker;
      return cfg;
    }()),
    xnap_db(xnap_repository_config{local_cu_cp_cfg, cu_cp_xnap_handler, test_logger})
  {
  }

  timer_manager            timers;
  manual_task_worker       ctrl_worker{128};
  ue_manager_config        ue_cfg;
  ue_manager_dependencies  ue_dependencies;
  ue_manager               ue_mng;
  dummy_cu_cp_xnap_handler cu_cp_xnap_handler{ue_mng};
  cu_cp_configuration      local_cu_cp_cfg;
  xnap_repository          xnap_db;

  gnb_id_t                default_gnb_id{.id = 411, .bit_length = 22};
  transport_layer_address default_peer_addr = transport_layer_address::create_from_string("127.0.0.1");
  guami_t default_guami{.plmn = plmn_identity::test_value(), .amf_set_id = 1, .amf_pointer = 1, .amf_region_id = 1};
  xnap_configuration xnap_cfg{.gnb_id           = default_gnb_id,
                              .tai_support_list = {default_supported_tracking_area},
                              .guami_list       = {default_guami}};

  gnb_id_t           peer_gnb_id{.id = 412, .bit_length = 22};
  xnap_configuration xnap_peer_cfg{.gnb_id           = peer_gnb_id,
                                   .tai_support_list = {default_supported_tracking_area},
                                   .guami_list       = {default_guami}};

  /// Runs the XN setup procedure against \c xnap, with the peer advertising a single served cell.
  void complete_xn_setup_with_served_cell(xnc_peer_index_t xnc_index, xnap_interface& xnap, pci_t served_pci)
  {
    xnap_db.connect_association(xnc_index, std::make_unique<dummy_xnap_message_notifier>(last_xnap_msg));

    async_task<bool>         t = xnap.handle_xn_setup_request_required();
    lazy_task_launcher<bool> t_launcher(t);

    const nr_cell_global_id_t served_cgi{plmn_identity::test_value(), nr_cell_identity::create(0x19b0).value()};
    xnap_message setup_resp = generate_xn_setup_response_with_served_cell(xnap_peer_cfg, served_pci, served_cgi);
    xnap.handle_message(setup_resp);

    ASSERT_TRUE(t.ready());
    ASSERT_TRUE(t.get()) << "XN setup procedure failed";
  }

  xnap_message last_xnap_msg;
};

//----------------------------------------------------------------------------------//
// XNAP repository tests                                                            //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_xnap_repository_test,
       when_adding_xnap_then_xnap_is_added_to_repository_and_can_be_retrieved_by_index_and_peer_addr)
{
  // Add XNAP.
  xnap_interface* xnap = xnap_db.add_xnap(xnc_peer_index_t::min, {default_peer_addr}, xnap_cfg);
  ASSERT_TRUE(xnap != nullptr) << "Failed to add XNAP to repository";

  ASSERT_EQ(xnap_db.get_nof_xnaps(), 1U) << "Unexpected number of XNAPs in repository after adding XNAP";

  // Retrieve XNAP by index.
  xnap_interface* retrieved_xnap = xnap_db.find_xnap(xnc_peer_index_t::min);
  ASSERT_TRUE(retrieved_xnap != nullptr) << "Failed to retrieve XNAP by index";
  ASSERT_EQ(retrieved_xnap, xnap) << "Retrieved XNAP does not match added XNAP";

  // Retrieve XNAP by peer address.
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(default_peer_addr)) << "Failed to retrieve XNAP by peer address";
}

TEST_F(cu_cp_xnap_repository_test, when_multihomed_peer_added_then_find_xnap_matches_any_of_its_addresses)
{
  // Add a multihomed XNAP peer with three addresses.
  const transport_layer_address addr_a = transport_layer_address::create_from_string("127.0.0.1");
  const transport_layer_address addr_b = transport_layer_address::create_from_string("127.0.0.2");
  const transport_layer_address addr_c = transport_layer_address::create_from_string("127.0.0.3");

  xnap_interface* xnap = xnap_db.add_xnap(xnc_peer_index_t::min, {addr_a, addr_b, addr_c}, xnap_cfg);
  ASSERT_TRUE(xnap != nullptr) << "Failed to add multihomed XNAP to repository";

  // SCTP COMM_UP notification may report any of the configured addresses, lookup must find on any of them.
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(addr_a)) << "Failed to retrieve XNAP by first address";
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(addr_b)) << "Failed to retrieve XNAP by second address";
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap(addr_c)) << "Failed to retrieve XNAP by third address";

  // An unrelated address must not match.
  const transport_layer_address unrelated = transport_layer_address::create_from_string("127.0.0.99");
  ASSERT_EQ(xnc_peer_index_t::invalid, xnap_db.find_xnap(unrelated)) << "Unexpected match for unrelated address";
}

TEST_F(cu_cp_xnap_repository_test, when_peer_serves_a_cell_then_xnap_is_found_by_its_pci)
{
  const pci_t served_pci = 42;

  xnap_interface* xnap = xnap_db.add_xnap(xnc_peer_index_t::min, {default_peer_addr}, xnap_cfg);
  ASSERT_TRUE(xnap != nullptr) << "Failed to add XNAP to repository";

  // Served cells are only known once the peer advertised them at XN setup.
  ASSERT_FALSE(xnap_db.find_xnap_index_by_served_pci(served_pci).has_value())
      << "Unexpected match before the XN setup completed";

  ASSERT_NO_FATAL_FAILURE(complete_xn_setup_with_served_cell(xnc_peer_index_t::min, *xnap, served_pci));

  // A UE reestablishing on this PCI must resolve to the peer holding its context (TS 38.423 section 8.2.4).
  ASSERT_EQ(xnc_peer_index_t::min, xnap_db.find_xnap_index_by_served_pci(served_pci))
      << "Failed to retrieve XNAP index by served cell PCI";
  ASSERT_FALSE(xnap_db.find_xnap_index_by_served_pci(static_cast<pci_t>(served_pci + 1)).has_value())
      << "Unexpected match for a PCI no peer serves";
}

//----------------------------------------------------------------------------------//
// Inter-CU CHO candidate cancellation                                              //
//----------------------------------------------------------------------------------//

/// A source CU-CP preparing CHO candidates at two different peers gets a Target NG-RAN node UE XnAP ID from each.
/// Those IDs are only unique per Xn interface, so freshly started peers routinely hand out the same one. The winner
/// must therefore be identified by target cell: identifying it by XNAP UE ID matches every candidate and cancels none.
TEST_F(cu_cp_xnap_repository_test, when_candidates_at_two_peers_share_a_peer_ue_id_then_only_the_loser_is_cancelled)
{
  const xnc_peer_index_t xnc_index_a = xnc_peer_index_t::min;
  const xnc_peer_index_t xnc_index_b =
      uint_to_xnc_peer_index(static_cast<std::underlying_type_t<xnc_peer_index_t>>(xnc_peer_index_t::min) + 1);

  ASSERT_NE(xnap_db.add_xnap(xnc_index_a, {default_peer_addr}, xnap_cfg), nullptr);
  ASSERT_NE(xnap_db.add_xnap(xnc_index_b, {transport_layer_address::create_from_string("127.0.0.2")}, xnap_cfg),
            nullptr);

  const cu_cp_ue_index_t source_ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  ASSERT_NE(source_ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(
      ue_mng.update_ue_context(source_ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti_t::MIN_CRNTI, MIN_DU_CELL_INDEX));
  cu_cp_ue* source_ue = ue_mng.find_du_ue(source_ue_index);
  ASSERT_NE(source_ue, nullptr);

  const nr_cell_global_id_t cgi_a{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 1).value()};
  const nr_cell_global_id_t cgi_b{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 2).value()};

  // Both peers allocated the same Target NG-RAN node UE XnAP ID, as two freshly started CU-CPs do.
  const peer_xnap_ue_id_t shared_peer_ue_id = peer_xnap_ue_id_t::min;

  source_ue->get_cho_context().emplace();
  for (const auto& [cgi, xnc_index] : {std::make_pair(cgi_a, xnc_index_a), std::make_pair(cgi_b, xnc_index_b)}) {
    cu_cp_cho_candidate candidate;
    candidate.target_cgi      = cgi;
    candidate.xnc_index       = xnc_index;
    candidate.peer_xnap_ue_id = shared_peer_ue_id;
    source_ue->get_cho_context()->candidates.push_back(candidate);
  }

  // The UE accessed cell A, so only the candidate at cell B must be cancelled.
  const unsigned cancelled = cancel_cho_candidates(*source_ue, ue_mng, &xnap_db, cu_cp_ue_index_t::invalid, cgi_a);

  ASSERT_EQ(cancelled, 1) << "expected the candidate at the losing peer to be cancelled";
}

/// TS 38.423 Sections 8.2.3.2 and 8.2.9.2 identify a UE-associated signalling connection by the Source *and* Target
/// NG-RAN node UE XnAP IDs, so a peer that answers each candidate with its own Target ID puts them on separate
/// connections. The implicit cancellation of Section 8.2.8.2 does not reach across those, and nothing at the target
/// releases the sibling, so it needs an explicit Handover Cancel.
TEST_F(cu_cp_xnap_repository_test, when_a_sibling_at_the_winners_node_has_its_own_target_ue_id_then_it_is_cancelled)
{
  const xnc_peer_index_t xnc_index = xnc_peer_index_t::min;
  ASSERT_NE(xnap_db.add_xnap(xnc_index, {default_peer_addr}, xnap_cfg), nullptr);

  const cu_cp_ue_index_t source_ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  ASSERT_NE(source_ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(
      ue_mng.update_ue_context(source_ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti_t::MIN_CRNTI, MIN_DU_CELL_INDEX));
  cu_cp_ue* source_ue = ue_mng.find_du_ue(source_ue_index);
  ASSERT_NE(source_ue, nullptr);

  const nr_cell_global_id_t cgi_a{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 1).value()};
  const nr_cell_global_id_t cgi_b{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 2).value()};

  // Both candidate cells are served by one peer, which gave each its own Target NG-RAN node UE XnAP ID.
  const peer_xnap_ue_id_t peer_ue_id_a = peer_xnap_ue_id_t::min;
  const peer_xnap_ue_id_t peer_ue_id_b = uint_to_peer_xnap_ue_id(peer_xnap_ue_id_to_uint(peer_xnap_ue_id_t::min) + 1);

  source_ue->get_cho_context().emplace();
  for (const auto& [cgi, peer_ue_id] : {std::make_pair(cgi_a, peer_ue_id_a), std::make_pair(cgi_b, peer_ue_id_b)}) {
    cu_cp_cho_candidate candidate;
    candidate.target_cgi      = cgi;
    candidate.xnc_index       = xnc_index;
    candidate.peer_xnap_ue_id = peer_ue_id;
    source_ue->get_cho_context()->candidates.push_back(candidate);
  }

  // The UE accessed cell A; cell B is a separate signalling connection at the same node and must be cancelled.
  const unsigned cancelled = cancel_cho_candidates(*source_ue, ue_mng, &xnap_db, cu_cp_ue_index_t::invalid, cgi_a);

  ASSERT_EQ(cancelled, 1) << "the sibling at the winner's peer was not cancelled";
}

/// TS 38.423 Section 8.2.1.1 lets a target prepare several candidate cells of one UE on a single UE-associated
/// signalling connection, told apart by target cell. Section 8.2.8.2 then has the source consider every other
/// preparation on that connection cancelled once the Handover Success arrives, so those must not be cancelled again.
TEST_F(cu_cp_xnap_repository_test, when_a_sibling_shares_the_winners_signalling_connection_then_it_is_not_cancelled)
{
  const xnc_peer_index_t xnc_index = xnc_peer_index_t::min;
  ASSERT_NE(xnap_db.add_xnap(xnc_index, {default_peer_addr}, xnap_cfg), nullptr);

  const cu_cp_ue_index_t source_ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  ASSERT_NE(source_ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(
      ue_mng.update_ue_context(source_ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti_t::MIN_CRNTI, MIN_DU_CELL_INDEX));
  cu_cp_ue* source_ue = ue_mng.find_du_ue(source_ue_index);
  ASSERT_NE(source_ue, nullptr);

  const nr_cell_global_id_t cgi_a{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 1).value()};
  const nr_cell_global_id_t cgi_b{plmn_identity::test_value(), nr_cell_identity::create({1, 22}, 2).value()};

  // Both candidate cells were prepared on one signalling connection, so they share the Target NG-RAN node UE XnAP ID.
  const peer_xnap_ue_id_t shared_peer_ue_id = peer_xnap_ue_id_t::min;

  source_ue->get_cho_context().emplace();
  for (const nr_cell_global_id_t& cgi : {cgi_a, cgi_b}) {
    cu_cp_cho_candidate candidate;
    candidate.target_cgi      = cgi;
    candidate.xnc_index       = xnc_index;
    candidate.peer_xnap_ue_id = shared_peer_ue_id;
    source_ue->get_cho_context()->candidates.push_back(candidate);
  }

  // The UE accessed cell A; cell B is on the same connection, so the target already considers it cancelled.
  const unsigned cancelled = cancel_cho_candidates(*source_ue, ue_mng, &xnap_db, cu_cp_ue_index_t::invalid, cgi_a);

  ASSERT_EQ(cancelled, 0) << "a sibling on the winner's own signalling connection must not be cancelled";
}
