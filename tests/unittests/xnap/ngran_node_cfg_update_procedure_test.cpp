// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/xnap/procedures/ngran_node_cfg_update_asn1_helpers.h"
#include "xnap_test_helpers.h"
#include "xnap_test_messages.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::ocucp;

/// Fixture class for the NG-RAN node configuration update tests.
class ngran_node_cfg_update_procedure_test : public xnap_test
{
protected:
  cu_cp_served_cell_info make_served_cell(pci_t pci, unsigned cell_idx)
  {
    return generate_served_cell_info(
        pci, nr_cell_global_id_t{local_plmn, nr_cell_identity::create(local_gnb_id, cell_idx).value()}, local_tac);
  }

  /// Runs an NG-RAN node configuration update to completion and returns the message the peer received.
  xnap_message run_cfg_update()
  {
    async_task<bool>         t = xnap->handle_served_cells_update_required();
    lazy_task_launcher<bool> t_launcher(t);

    const xnap_message cfg_update = get_last_message();
    xnap->handle_message(generate_asn1_ngran_node_cfg_update_ack());
    EXPECT_TRUE(t.ready());
    EXPECT_TRUE(t.get());

    return cfg_update;
  }
};

TEST_F(ngran_node_cfg_update_procedure_test, when_a_new_cell_is_served_then_it_is_reported_to_the_peer)
{
  ASSERT_TRUE(run_xn_setup(xnap_peer_cfg));

  const cu_cp_served_cell_info served_cell = make_served_cell(2, 1);
  cu_cp_notifier.set_served_cells({served_cell});

  const xnap_message cfg_update = run_cfg_update();

  ASSERT_EQ(cfg_update.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(cfg_update.pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::ngran_node_cfg_upd);

  const auto& asn1_gnb = cfg_update.pdu.init_msg().value.ngran_node_cfg_upd()->cfg_upd_init_node_choice.gnb();
  ASSERT_TRUE(asn1_gnb.served_cells_to_upd_nr_present);
  ASSERT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_add_nr.size(), 1);

  const auto& asn1_cell_info = asn1_gnb.served_cells_to_upd_nr.served_cells_to_add_nr[0].served_cell_info_nr;
  EXPECT_EQ(asn1_cell_info.nr_pci, served_cell.nr_pci);
  EXPECT_EQ(asn1_to_cgi(asn1_cell_info.cell_id), served_cell.nr_cgi);
  EXPECT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_delete_nr.size(), 0);
}

TEST_F(ngran_node_cfg_update_procedure_test, when_a_cell_stops_being_served_then_it_is_reported_as_deleted)
{
  const cu_cp_served_cell_info served_cell = make_served_cell(2, 1);
  cu_cp_notifier.set_served_cells({served_cell});
  ASSERT_TRUE(run_xn_setup(xnap_peer_cfg));

  cu_cp_notifier.set_served_cells({});

  const xnap_message cfg_update = run_cfg_update();

  const auto& asn1_gnb = cfg_update.pdu.init_msg().value.ngran_node_cfg_upd()->cfg_upd_init_node_choice.gnb();
  ASSERT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_delete_nr.size(), 1);
  EXPECT_EQ(asn1_to_cgi(asn1_gnb.served_cells_to_upd_nr.served_cells_to_delete_nr[0]), served_cell.nr_cgi);
  EXPECT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_add_nr.size(), 0);
}

TEST_F(ngran_node_cfg_update_procedure_test, when_the_pci_of_a_served_cell_changes_then_it_is_reported_as_modified)
{
  cu_cp_notifier.set_served_cells({make_served_cell(2, 1)});
  ASSERT_TRUE(run_xn_setup(xnap_peer_cfg));

  cu_cp_notifier.set_served_cells({make_served_cell(3, 1)});

  const xnap_message cfg_update = run_cfg_update();

  const auto& asn1_gnb = cfg_update.pdu.init_msg().value.ngran_node_cfg_upd()->cfg_upd_init_node_choice.gnb();
  ASSERT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_modify_nr.size(), 1);
  EXPECT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_modify_nr[0].served_cell_info_nr.nr_pci, 3);
  EXPECT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_add_nr.size(), 0);
}

TEST_F(ngran_node_cfg_update_procedure_test, when_the_carrier_of_a_served_cell_changes_then_it_is_reported_as_modified)
{
  cu_cp_notifier.set_served_cells({make_served_cell(2, 1)});
  ASSERT_TRUE(run_xn_setup(xnap_peer_cfg));

  cu_cp_served_cell_info retuned_cell                                       = make_served_cell(2, 1);
  std::get<cu_cp_tdd_info>(retuned_cell.nr_mode_info).nr_freq_info.nr_arfcn = 636666;
  cu_cp_notifier.set_served_cells({retuned_cell});

  const xnap_message cfg_update = run_cfg_update();

  const auto& asn1_gnb = cfg_update.pdu.init_msg().value.ngran_node_cfg_upd()->cfg_upd_init_node_choice.gnb();
  ASSERT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_modify_nr.size(), 1);
  const auto& asn1_cell_info = asn1_gnb.served_cells_to_upd_nr.served_cells_to_modify_nr[0].served_cell_info_nr;
  EXPECT_EQ(asn1_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn, 636666);
  EXPECT_EQ(asn1_gnb.served_cells_to_upd_nr.served_cells_to_add_nr.size(), 0);
}

TEST_F(ngran_node_cfg_update_procedure_test, when_the_served_cells_are_unchanged_then_no_update_is_sent)
{
  cu_cp_notifier.set_served_cells({make_served_cell(2, 1)});
  ASSERT_TRUE(run_xn_setup(xnap_peer_cfg));

  async_task<bool>         t = xnap->handle_served_cells_update_required();
  lazy_task_launcher<bool> t_launcher(t);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get());
  ASSERT_EQ(get_last_message().pdu.init_msg().value.type(),
            asn1::xnap::xnap_elem_procs_o::init_msg_c::types_opts::xn_setup_request)
      << "The Xn-C peer received a message although the served cells did not change";
}

TEST_F(ngran_node_cfg_update_procedure_test, when_the_peer_reports_its_cells_then_the_update_is_acknowledged)
{
  ASSERT_TRUE(run_xn_setup(xnap_peer_cfg));

  xnap_served_cells_update update;
  update.cells_to_add.push_back(generate_served_cell_info(
      5, nr_cell_global_id_t{local_plmn, nr_cell_identity::create(gnb_id_t{412, 22}, 1).value()}));
  xnap->handle_message(generate_asn1_ngran_node_cfg_update(update));

  const xnap_message ack = get_last_message();
  ASSERT_EQ(ack.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(ack.pdu.successful_outcome().value.type(),
            asn1::xnap::xnap_elem_procs_o::successful_outcome_c::types_opts::ngran_node_cfg_upd_ack);
}

TEST_F(ngran_node_cfg_update_procedure_test, when_no_xn_setup_was_run_then_a_reported_update_is_rejected)
{
  xnap->handle_message(generate_asn1_ngran_node_cfg_update(xnap_served_cells_update{}));

  const xnap_message fail = get_last_message();
  ASSERT_EQ(fail.pdu.type(), asn1::xnap::xn_ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(fail.pdu.unsuccessful_outcome().value.type(),
            asn1::xnap::xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::ngran_node_cfg_upd_fail);
}

/// The cells reported by a peer are added, replaced and removed in the cells stored for it.
TEST(ngran_node_cfg_update_asn1_helpers_test, reported_cells_are_applied_to_the_cells_stored_for_the_peer)
{
  const nr_cell_global_id_t cgi1{plmn_identity::test_value(), nr_cell_identity::create(gnb_id_t{412, 22}, 1).value()};
  const nr_cell_global_id_t cgi2{plmn_identity::test_value(), nr_cell_identity::create(gnb_id_t{412, 22}, 2).value()};

  std::vector<cu_cp_served_cell_info> peer_cells;
  peer_cells.push_back(generate_served_cell_info(1, cgi1));

  xnap_served_cells_update update;
  update.cells_to_add.push_back(generate_served_cell_info(2, cgi2));
  update.cells_to_modify.push_back(generate_served_cell_info(3, cgi1));

  const xnap_message cfg_update = generate_asn1_ngran_node_cfg_update(update);
  update_peer_served_cells(
      peer_cells,
      cfg_update.pdu.init_msg().value.ngran_node_cfg_upd()->cfg_upd_init_node_choice.gnb().served_cells_to_upd_nr);

  ASSERT_EQ(peer_cells.size(), 2);
  EXPECT_EQ(peer_cells[0].nr_cgi, cgi1);
  EXPECT_EQ(peer_cells[0].nr_pci, 3) << "The reported cell did not replace the stored one";
  EXPECT_EQ(peer_cells[1].nr_cgi, cgi2);

  xnap_served_cells_update removal;
  removal.cells_to_delete.push_back(cgi1);

  const xnap_message cfg_update_removal = generate_asn1_ngran_node_cfg_update(removal);
  update_peer_served_cells(peer_cells,
                           cfg_update_removal.pdu.init_msg()
                               .value.ngran_node_cfg_upd()
                               ->cfg_upd_init_node_choice.gnb()
                               .served_cells_to_upd_nr);

  ASSERT_EQ(peer_cells.size(), 1);
  EXPECT_EQ(peer_cells[0].nr_cgi, cgi2);
}
