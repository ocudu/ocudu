// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class cu_cp_cell_command_handler_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_cell_command_handler_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    run_ng_setup();

    auto ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();

    // F1 setup with a single served cell. served_cell_item_info defaults give us a known CGI.
    test_helpers::served_cell_item_info cell;
    served_cgi = nr_cell_global_id_t{cell.plmn_id, cell.nci};
    EXPECT_TRUE(run_f1_setup(du_idx, int_to_gnb_du_id(0x11), {cell}));
  }

  /// Pop the F1AP gNB-CU Configuration Update emitted by the CU-CP toward the DU. Returns false if no
  /// such message arrives.
  bool pop_cu_cfg_upd(f1ap_message& out)
  {
    if (!wait_for_f1ap_tx_pdu(du_idx, out)) {
      return false;
    }
    if (out.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types::init_msg) {
      return false;
    }
    return out.pdu.init_msg().proc_code == ASN1_F1AP_ID_GNB_CU_CFG_UPD;
  }

  /// Build a gNB-CU Configuration Update acknowledgement matching the transaction of the given request.
  f1ap_message make_ack_for(const f1ap_message& request)
  {
    f1ap_message ack = test_helpers::generate_gnb_cu_configuration_update_acknowledgement({});
    ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack()->transaction_id =
        request.pdu.init_msg().value.gnb_cu_cfg_upd()->transaction_id;
    return ack;
  }

  unsigned            du_idx{0};
  nr_cell_global_id_t served_cgi;
};

TEST_F(cu_cp_cell_command_handler_test, when_deactivate_cell_then_cfg_upd_carries_cgi_and_completes_on_du_ack)
{
  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(served_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  // CU-CP emits a gNB-CU Configuration Update carrying the served CGI in the deactivate list.
  f1ap_message cu_cfg_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(cu_cfg_upd)) << "CU-CP did not emit gNB-CU Configuration Update";
  const auto& upd_ies = cu_cfg_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_deactiv_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_deactiv_list.size(), 1U);
  const auto& deactiv_item = upd_ies->cells_to_be_deactiv_list[0].value().cells_to_be_deactiv_list_item();
  // NCI uniquely identifies the served cell; the ASN.1 3-octet PLMN round-trip is not load-bearing here.
  ASSERT_EQ(deactiv_item.nr_cgi.nr_cell_id.to_number(), served_cgi.nci.value());

  // DU acks the update; the procedure completes with success.
  get_du(du_idx).push_ul_pdu(make_ack_for(cu_cfg_upd));
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return launcher.ready(); }, false));
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_TRUE(launcher.result.value().success);
}

TEST_F(cu_cp_cell_command_handler_test, when_activate_cell_then_cfg_upd_carries_cgi_and_completes_on_du_ack)
{
  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.activate_cell(served_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  f1ap_message cu_cfg_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(cu_cfg_upd)) << "CU-CP did not emit gNB-CU Configuration Update";
  const auto& upd_ies = cu_cfg_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_activ_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_activ_list.size(), 1U);
  const auto& activ_item = upd_ies->cells_to_be_activ_list[0].value().cells_to_be_activ_list_item();
  ASSERT_EQ(activ_item.nr_cgi.nr_cell_id.to_number(), served_cgi.nci.value());

  get_du(du_idx).push_ul_pdu(make_ack_for(cu_cfg_upd));
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return launcher.ready(); }, false));
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_TRUE(launcher.result.value().success);
}

TEST_F(cu_cp_cell_command_handler_test, when_cgi_is_unknown_then_command_fails_without_f1ap_traffic)
{
  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  // A CGI no connected DU serves: same PLMN, different NCI.
  nr_cell_global_id_t unknown_cgi{served_cgi.plmn_id, nr_cell_identity::create(served_cgi.nci.value() + 1).value()};

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(unknown_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  // Validation fails without DU interaction.
  ASSERT_TRUE(launcher.ready()) << "Unknown CGI should fail synchronously";
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_FALSE(launcher.result.value().success);

  // And no F1AP gNB-CU Configuration Update goes out toward the DU.
  f1ap_message unused;
  ASSERT_FALSE(pop_cu_cfg_upd(unused)) << "No F1AP traffic expected for an unknown CGI";
}

TEST_F(cu_cp_cell_command_handler_test, when_du_rejects_cfg_upd_then_command_fails)
{
  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(served_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  f1ap_message cu_cfg_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(cu_cfg_upd));

  // DU rejects the configuration update. Echo the request's transaction id so the failure matches
  // the pending procedure.
  f1ap_message fail = test_helpers::generate_gnb_cu_configuration_update_failure();
  fail.pdu.unsuccessful_outcome().value.gnb_cu_cfg_upd_fail()->transaction_id =
      cu_cfg_upd.pdu.init_msg().value.gnb_cu_cfg_upd()->transaction_id;
  get_du(du_idx).push_ul_pdu(fail);

  // CU-CP should resolve the procedure as failed.
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return launcher.ready(); }, false));
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_FALSE(launcher.result.value().success);
}

TEST_F(cu_cp_cell_command_handler_test, when_activate_follows_deactivate_then_deactivated_cell_is_found)
{
  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  // Lock the cell first. On the deactivate ack the cell leaves the DU's served-cell view.
  {
    async_task<cu_cp_cell_command_response>         deact_task = cell_cmd.deactivate_cell(served_cgi);
    lazy_task_launcher<cu_cp_cell_command_response> deact_launcher(deact_task);

    f1ap_message deact_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(deact_upd));
    get_du(du_idx).push_ul_pdu(make_ack_for(deact_upd));

    ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return deact_launcher.ready(); }, false));
    ASSERT_TRUE(deact_launcher.result.has_value());
    ASSERT_TRUE(deact_launcher.result.value().success);
  }

  // Unlock. activate_cell must locate the now-deactivated cell via the any-state DU lookup; the
  // strict served-cells lookup no longer finds it. Without that lookup no cfg update is emitted.
  async_task<cu_cp_cell_command_response>         act_task = cell_cmd.activate_cell(served_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> act_launcher(act_task);

  f1ap_message act_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(act_upd)) << "Activate after deactivate emitted no cfg update; cell lookup failed";
  const auto& upd_ies = act_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_activ_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_activ_list.size(), 1U);

  get_du(du_idx).push_ul_pdu(make_ack_for(act_upd));
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return act_launcher.ready(); }, false));
  ASSERT_TRUE(act_launcher.result.has_value());
  EXPECT_TRUE(act_launcher.result.value().success);
}

TEST_F(cu_cp_cell_command_handler_test, when_deactivate_cell_with_attached_ue_then_cu_releases_ue_before_deactivating)
{
  // A UE camped on the served cell.
  auto cu_up_idx = connect_new_cu_up();
  ASSERT_TRUE(cu_up_idx.has_value());
  ASSERT_TRUE(run_e1_setup(cu_up_idx.value()));

  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  ASSERT_TRUE(connect_new_ue(du_idx, du_ue_id, to_rnti(0x4601)));

  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(served_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  // The CU-CP releases the cell's UE itself (rather than relying on the DU to drain it): an F1AP UE Context Release
  // Command goes out first, and no gNB-CU Configuration Update is emitted until the release completes.
  f1ap_message release_cmd;
  ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, release_cmd));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(release_cmd))
      << "CU-CP should release the cell's UE before deactivating the cell";

  // The DU acknowledges the UE release; only then should the deactivation cfg update be emitted.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(release_cmd));

  f1ap_message cu_cfg_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(cu_cfg_upd)) << "Deactivation cfg update should follow the UE release";
  const auto& upd_ies = cu_cfg_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_deactiv_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_deactiv_list.size(), 1U);

  get_du(du_idx).push_ul_pdu(make_ack_for(cu_cfg_upd));
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return launcher.ready(); }, false));
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_TRUE(launcher.result.value().success);
}

TEST_F(cu_cp_cell_command_handler_test,
       when_deactivate_cell_with_multiple_ues_then_all_are_released_before_deactivating)
{
  auto cu_up_idx = connect_new_cu_up();
  ASSERT_TRUE(cu_up_idx.has_value());
  ASSERT_TRUE(run_e1_setup(cu_up_idx.value()));

  // Several UEs camped on the served cell.
  const unsigned nof_ues = 3;
  for (unsigned i = 0; i != nof_ues; ++i) {
    ASSERT_TRUE(connect_new_ue(du_idx, int_to_gnb_du_ue_f1ap_id(i), to_rnti(0x4601 + i)));
  }

  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(served_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  // The CU-CP releases every UE on the cell (one F1AP UE Context Release Command each) before deactivating.
  std::vector<f1ap_message> release_cmds(nof_ues);
  for (unsigned i = 0; i != nof_ues; ++i) {
    ASSERT_TRUE(wait_for_f1ap_tx_pdu(du_idx, release_cmds[i]))
        << "expected a UE Context Release Command for UE " << i << " before deactivation";
    ASSERT_TRUE(test_helpers::is_valid_ue_context_release_command(release_cmds[i]));
  }
  for (unsigned i = 0; i != nof_ues; ++i) {
    get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(release_cmds[i]));
  }

  // Only once all UEs are released does the deactivation cfg update go out.
  f1ap_message cu_cfg_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(cu_cfg_upd)) << "deactivation cfg update should follow all UE releases";

  get_du(du_idx).push_ul_pdu(make_ack_for(cu_cfg_upd));
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return launcher.ready(); }, false));
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_TRUE(launcher.result.value().success);
}

/// Fixture with two cells on a single DU, used to prove that deactivating one cell only releases that cell's UEs.
class cu_cp_cell_command_multicell_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_cell_command_multicell_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    run_ng_setup();

    auto ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();

    // The cell the simulated UE camps on: default CGI, which generate_init_ul_rrc_message_transfer targets.
    test_helpers::served_cell_item_info camped_cell;
    camped_cgi = nr_cell_global_id_t{camped_cell.plmn_id, camped_cell.nci};
    // A second cell on the same DU with a distinct CGI/PCI: the one we will lock (no UEs on it).
    test_helpers::served_cell_item_info other_cell;
    other_cell.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
    other_cell.pci = 7;
    other_cgi      = nr_cell_global_id_t{other_cell.plmn_id, other_cell.nci};

    EXPECT_TRUE(run_f1_setup(du_idx, int_to_gnb_du_id(0x11), {camped_cell, other_cell}));

    auto cu_up_idx = connect_new_cu_up();
    EXPECT_TRUE(cu_up_idx.has_value());
    EXPECT_TRUE(run_e1_setup(cu_up_idx.value()));
    EXPECT_TRUE(connect_new_ue(du_idx, int_to_gnb_du_ue_f1ap_id(0), to_rnti(0x4601)));
  }

  bool pop_cu_cfg_upd(f1ap_message& out)
  {
    if (!wait_for_f1ap_tx_pdu(du_idx, out)) {
      return false;
    }
    if (out.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types::init_msg) {
      return false;
    }
    return out.pdu.init_msg().proc_code == ASN1_F1AP_ID_GNB_CU_CFG_UPD;
  }

  f1ap_message make_ack_for(const f1ap_message& request)
  {
    f1ap_message ack = test_helpers::generate_gnb_cu_configuration_update_acknowledgement({});
    ack.pdu.successful_outcome().value.gnb_cu_cfg_upd_ack()->transaction_id =
        request.pdu.init_msg().value.gnb_cu_cfg_upd()->transaction_id;
    return ack;
  }

  unsigned            du_idx{0};
  nr_cell_global_id_t camped_cgi;
  nr_cell_global_id_t other_cgi;
};

TEST_F(cu_cp_cell_command_multicell_test, when_deactivate_cell_then_ues_on_other_cells_are_not_released)
{
  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  // The UE is attached before we lock the other cell.
  ASSERT_EQ(get_cu_cp().get_metrics_handler().request_metrics_report().ues.size(), 1U);

  // Lock the cell that has no UEs. The UE on the camped cell must be left alone.
  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(other_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  // No UE is released, so the very first F1AP PDU is the deactivation cfg update, carrying the locked cell only.
  f1ap_message cu_cfg_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(cu_cfg_upd)) << "a UE was released when locking a different cell";
  const auto& upd_ies = cu_cfg_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_deactiv_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_deactiv_list.size(), 1U);
  ASSERT_EQ(upd_ies->cells_to_be_deactiv_list[0].value().cells_to_be_deactiv_list_item().nr_cgi.nr_cell_id.to_number(),
            other_cgi.nci.value());

  get_du(du_idx).push_ul_pdu(make_ack_for(cu_cfg_upd));
  ASSERT_TRUE(tick_until(std::chrono::milliseconds{500}, [&]() { return launcher.ready(); }, false));
  ASSERT_TRUE(launcher.result.has_value());
  EXPECT_TRUE(launcher.result.value().success);

  // The UE on the camped cell survived the lock of the other cell.
  EXPECT_EQ(get_cu_cp().get_metrics_handler().request_metrics_report().ues.size(), 1U);
}
