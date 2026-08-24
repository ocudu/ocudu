// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "lib/xnap/procedures/ngran_node_cfg_update_asn1_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/xnap/xnap_test_message_validators.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/f1ap/common/interface_management.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Fixture for the report of the cells this node serves to an XN-C peer (TS 38.423 section 8.4.1). The fixture leaves
/// a single DU connected, with the cell it reported at F1 setup advertised to the peer.
class cu_cp_xn_served_cells_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_xn_served_cells_test() :
    cu_cp_test_environment({/* max nof cu-ups */ 8,
                            /* max nof dus */ 8,
                            /* max nof ues */ 8192,
                            /* max nof drbs per ue */ 8,
                            /* amf config */ {{default_supported_tracking_area}},
                            /* trigger ho from measurements */ true,
                            /* enable rrc inactive */ false,
                            /* enable xnc peer */ true})
  {
    run_ng_setup();

    // Run XN setup to completion.
    run_xn_setup();

    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(run_f1_setup(du_idx, int_to_gnb_du_id(0x11), {served_cell}));
  }

  /// Acknowledge the next gNB-CU Configuration Update the CU-CP sends to the DU.
  bool ack_gnb_cu_configuration_update()
  {
    f1ap_message cu_cfg_upd;
    if (!wait_for_f1ap_tx_pdu(du_idx, cu_cfg_upd)) {
      return false;
    }
    f1ap_message ack = test_helpers::generate_gnb_cu_configuration_update_acknowledgement({});
    ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack()->transaction_id =
        cu_cfg_upd.pdu.init_msg().value.gnb_cu_cfg_upd()->transaction_id;
    get_du(du_idx).push_ul_pdu(ack);
    return true;
  }

  /// Pop the NG-RAN Node Configuration Update the CU-CP sent to the XN-C peer and acknowledge it.
  bool pop_ngran_node_cfg_update(xnap_message& out)
  {
    if (!wait_for_xnap_tx_pdu(xnc_peer_idx, out)) {
      return false;
    }
    if (!test_helpers::is_pdu_type(out, asn1::xnap::xnap_elem_procs_o::init_msg_c::types::ngran_node_cfg_upd)) {
      return false;
    }
    get_xnc_cu_cp(xnc_peer_idx).push_tx_pdu(generate_asn1_ngran_node_cfg_update_ack());
    return true;
  }

  /// Get the reported cell changes of an NG-RAN Node Configuration Update.
  static const asn1::xnap::served_cells_to_upd_nr_s& get_served_cells_to_update(const xnap_message& cfg_update)
  {
    return cfg_update.pdu.init_msg().value.ngran_node_cfg_upd()->cfg_upd_init_node_choice.gnb().served_cells_to_upd_nr;
  }

  static constexpr unsigned           xnc_peer_idx = 0;
  test_helpers::served_cell_item_info served_cell;
  unsigned                            du_idx = 0;
};

TEST_F(cu_cp_xn_served_cells_test, when_a_du_completes_f1_setup_then_its_cells_are_reported_to_the_peer)
{
  const auto& asn1_cells_to_upd = get_served_cells_to_update(last_ngran_node_cfg_update);
  ASSERT_EQ(asn1_cells_to_upd.served_cells_to_add_nr.size(), 1);

  const auto& asn1_cell_info = asn1_cells_to_upd.served_cells_to_add_nr[0].served_cell_info_nr;
  EXPECT_EQ(asn1_cell_info.nr_pci, served_cell.pci);
  EXPECT_EQ(asn1_cell_info.cell_id.nr_ci.to_number(), served_cell.nci.value());
  EXPECT_EQ(asn1_cell_info.tac.to_number(), served_cell.tac);

  // The carrier of the cell is what a peer needs to derive keys for a UE that accessed it.
  ASSERT_EQ(asn1_cell_info.nr_mode_info.type(), asn1::xnap::nr_mode_info_c::types_opts::tdd);
  EXPECT_EQ(asn1_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn, served_cell.nr_arfcn.value());
  EXPECT_EQ(asn1_cell_info.nr_mode_info.tdd().nr_transmisson_bw.nr_scs, asn1::xnap::nr_scs_opts::scs30);
}

TEST_F(cu_cp_xn_served_cells_test, when_a_cell_is_deactivated_then_it_is_reported_as_deleted_to_the_peer)
{
  const nr_cell_global_id_t served_cgi{served_cell.plmn_id, served_cell.nci};

  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  launched_cu_cp_task<cu_cp_cell_command_response> deactivation{*this,
                                                                [&]() { return cell_cmd.deactivate_cell(served_cgi); }};

  // Taking a cell down bars it first and deactivates it once the UEs are released, each with its own gNB-CU
  // Configuration Update the DU acknowledges.
  ASSERT_TRUE(ack_gnb_cu_configuration_update()) << "CU-CP did not bar the cell";
  ASSERT_TRUE(ack_gnb_cu_configuration_update()) << "CU-CP did not deactivate the cell";
  EXPECT_TRUE(wait_for_task_result(deactivation).success);

  xnap_message cfg_update;
  ASSERT_TRUE(pop_ngran_node_cfg_update(cfg_update)) << "CU-CP did not report the deactivated cell to the XN-C peer";

  const auto& asn1_cells_to_upd = get_served_cells_to_update(cfg_update);
  ASSERT_EQ(asn1_cells_to_upd.served_cells_to_delete_nr.size(), 1);
  EXPECT_EQ(asn1_cells_to_upd.served_cells_to_delete_nr[0].nr_ci.to_number(), served_cell.nci.value());
  EXPECT_EQ(asn1_cells_to_upd.served_cells_to_add_nr.size(), 0);
}

TEST_F(cu_cp_xn_served_cells_test, when_a_du_disconnects_then_its_cells_are_reported_as_deleted_to_the_peer)
{
  ASSERT_TRUE(drop_du_connection(du_idx));

  xnap_message cfg_update;
  ASSERT_TRUE(pop_ngran_node_cfg_update(cfg_update)) << "CU-CP did not report the cells of the removed DU to the XN-C "
                                                        "peer";

  const auto& asn1_cells_to_upd = get_served_cells_to_update(cfg_update);
  ASSERT_EQ(asn1_cells_to_upd.served_cells_to_delete_nr.size(), 1);
  EXPECT_EQ(asn1_cells_to_upd.served_cells_to_delete_nr[0].nr_ci.to_number(), served_cell.nci.value());
}
