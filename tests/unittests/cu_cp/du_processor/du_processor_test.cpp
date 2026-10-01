// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../du_processor_test_messages.h"
#include "du_processor_test_helpers.h"
#include "lib/cu_cp/du_processor/du_processor.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/ran/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* F1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful f1 setup procedure
TEST_F(du_processor_test, when_valid_f1setup_received_then_f1_setup_response_sent)
{
  // Pass F1 Setup Request to DU processor
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check response is F1SetupResponse
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp);
}

TEST_F(du_processor_test, when_du_served_cells_list_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing du served cells list
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list.clear();

  // Pass message to DU processor
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_gnb_du_sys_info_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing gnb du sys info
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  f1_setup_req.pdu.init_msg()
      .value.f1_setup_request()
      ->gnb_du_served_cells_list[0]
      .value()
      .gnb_du_served_cells_item()
      .gnb_du_sys_info_present = false;

  // Pass message to DU processor
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_max_nof_du_cells_exeeded_then_f1setup_rejected)
{
  // Generate F1SetupRequest with too many cells
  f1ap_message f1ap_msg = create_f1_setup_request_with_too_many_cells();

  // Pass message to DU processor
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1ap_msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

//////////////////////////////////////////////////////////////////////////////////////
/* UE creation                                                                      */
//////////////////////////////////////////////////////////////////////////////////////

class du_processor_ue_creation_test : public du_processor_test
{
protected:
  du_processor_ue_creation_test()
  {
    du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
        test_helpers::generate_f1_setup_request());
  }

  static f1ap_message create_valid_ue_creation_message()
  {
    return test_helpers::generate_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t{0}, rnti_t::MIN_CRNTI);
  }
};

TEST_F(du_processor_ue_creation_test, when_init_ul_rrc_message_is_valid_then_ue_added)
{
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      this->create_valid_ue_creation_message());

  ASSERT_EQ(ue_mng.get_nof_ues(), 1);
}

TEST_F(du_processor_ue_creation_test, when_init_ul_rrc_message_is_invalid_then_ue_is_not_added)
{
  f1ap_message msg = this->create_valid_ue_creation_message();
  msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->nr_cgi.nr_cell_id.from_number(1);

  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(msg);

  // Inject UE Context Release Complete.
  f1ap_message release_cmplt =
      test_helpers::generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t{0}, gnb_du_ue_f1ap_id_t{0});
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(release_cmplt);

  ASSERT_EQ(ue_mng.get_nof_ues(), 0);
}

//////////////////////////////////////////////////////////////////////////////////////
/* gNB-DU configuration update                                                      */
//////////////////////////////////////////////////////////////////////////////////////

/// Runs the F1 setup so the DU processor holds a DU configuration to update.
static void run_f1_setup(du_processor& du_proc, const std::vector<test_helpers::served_cell_item_info>& cells)
{
  du_proc.get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x11), cells));
}

TEST_F(du_processor_test, when_du_adds_a_cell_then_config_update_is_acknowledged_and_the_cell_is_activated)
{
  test_helpers::served_cell_item_info cell_a;
  run_f1_setup(*du_processor_obj, {cell_a});

  test_helpers::served_cell_item_info cell_b;
  cell_b.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
  cell_b.pci = 7;
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_gnb_du_configuration_update(int_to_gnb_du_id(0x11), {cell_b}));

  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_du_cfg_upd_ack);

  const auto& ack = f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.gnb_du_cfg_upd_ack();
  ASSERT_TRUE(ack->cells_to_be_activ_list_present) << "the added cell must be activated";
  ASSERT_EQ(ack->cells_to_be_activ_list.size(), 1);
  ASSERT_EQ(ack->cells_to_be_activ_list[0]->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.to_number(),
            cell_b.nci.value());

  ASSERT_EQ(du_processor_obj->get_context()->served_cells.size(), 2);
}

TEST_F(du_processor_test, when_du_deletes_a_cell_then_the_cu_cp_stops_serving_it)
{
  test_helpers::served_cell_item_info cell_a;
  test_helpers::served_cell_item_info cell_b;
  cell_b.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
  cell_b.pci = 7;
  run_f1_setup(*du_processor_obj, {cell_a, cell_b});
  ASSERT_EQ(du_processor_obj->get_context()->served_cells.size(), 2);

  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_gnb_du_configuration_update(
          int_to_gnb_du_id(0x11), {}, {}, {nr_cell_global_id_t{cell_b.plmn_id, cell_b.nci}}));

  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_du_cfg_upd_ack);
  ASSERT_EQ(du_processor_obj->get_context()->served_cells.size(), 1);
  ASSERT_EQ(du_processor_obj->get_context()->served_cells[0].cgi.nci, cell_a.nci);
}

TEST_F(du_processor_test, when_du_modifies_a_cell_then_the_new_configuration_is_stored)
{
  test_helpers::served_cell_item_info cell_a;
  run_f1_setup(*du_processor_obj, {cell_a});

  test_helpers::served_cell_item_info modified = cell_a;
  modified.pci                                 = 11;
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_gnb_du_configuration_update(
          int_to_gnb_du_id(0x11), {}, {{nr_cell_global_id_t{cell_a.plmn_id, cell_a.nci}, modified}}));

  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_du_cfg_upd_ack);
  ASSERT_EQ(du_processor_obj->get_context()->served_cells.size(), 1);
  ASSERT_EQ(du_processor_obj->get_context()->served_cells[0].pci, 11);
}

TEST_F(du_processor_test, when_a_modified_cell_can_no_longer_be_served_then_the_du_is_asked_to_deactivate_it)
{
  test_helpers::served_cell_item_info cell_a;
  run_f1_setup(*du_processor_obj, {cell_a});

  // The DU reconfigures the cell onto a PLMN the CU-CP does not serve.
  test_helpers::served_cell_item_info modified = cell_a;
  modified.plmn_id                             = plmn_identity::parse("00102").value();
  modified.sib1_str                            = test_helpers::create_sib1_hex_string(modified.plmn_id);
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_gnb_du_configuration_update(
          int_to_gnb_du_id(0x11), {}, {{nr_cell_global_id_t{cell_a.plmn_id, cell_a.nci}, modified}}));

  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_du_cfg_upd_ack);
  const auto& ack = f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.gnb_du_cfg_upd_ack();
  ASSERT_TRUE(ack->cells_to_be_deactiv_list_present) << "a cell the CU-CP stops serving must not stay on air";
  ASSERT_EQ(ack->cells_to_be_deactiv_list.size(), 1);
  ASSERT_EQ(ack->cells_to_be_deactiv_list[0]->cells_to_be_deactiv_list_item().nr_cgi.nr_cell_id.to_number(),
            cell_a.nci.value());
  ASSERT_TRUE(du_processor_obj->get_context()->served_cells.empty());
}

TEST_F(du_processor_test, when_the_du_deletes_a_cell_then_it_is_not_asked_to_deactivate_it)
{
  test_helpers::served_cell_item_info cell_a;
  test_helpers::served_cell_item_info cell_b;
  cell_b.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
  cell_b.pci = 7;
  run_f1_setup(*du_processor_obj, {cell_a, cell_b});

  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_gnb_du_configuration_update(
          int_to_gnb_du_id(0x11), {}, {}, {nr_cell_global_id_t{cell_b.plmn_id, cell_b.nci}}));

  const auto& ack = f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.gnb_du_cfg_upd_ack();
  ASSERT_FALSE(ack->cells_to_be_deactiv_list_present) << "the DU already stopped serving the cell";
}

TEST_F(du_processor_test, when_the_added_cell_has_an_unsupported_plmn_then_it_is_left_out)
{
  test_helpers::served_cell_item_info cell_a;
  run_f1_setup(*du_processor_obj, {cell_a});

  test_helpers::served_cell_item_info foreign_cell;
  foreign_cell.plmn_id  = plmn_identity::parse("00102").value();
  foreign_cell.nci      = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
  foreign_cell.pci      = 7;
  foreign_cell.sib1_str = test_helpers::create_sib1_hex_string(foreign_cell.plmn_id);
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      test_helpers::generate_gnb_du_configuration_update(int_to_gnb_du_id(0x11), {foreign_cell}));

  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_du_cfg_upd_ack)
      << "one cell the CU-CP cannot serve must not fail the whole update";
  const auto& ack = f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.gnb_du_cfg_upd_ack();
  ASSERT_FALSE(ack->cells_to_be_activ_list_present);
  ASSERT_EQ(du_processor_obj->get_context()->served_cells.size(), 1);
}
