// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_test_environment.h"
#include "test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

/// Base harness for CU-CP logical cell tests: a DU with two served cells, and helpers to run the F1 setup
/// manually so the F1 Setup Response (in particular its Cells to be Activated List) can be inspected.
class cu_cp_logical_cell_test_base : public cu_cp_test_environment
{
public:
  explicit cu_cp_logical_cell_test_base(cu_cp_test_env_params env_params) :
    cu_cp_test_environment(std::move(env_params))
  {
    run_ng_setup();

    // Two served cells on one DU: sector 0 (the default served_cell_item_info) and sector 1.
    cell_a_info     = test_helpers::served_cell_item_info{};
    cell_b_info     = test_helpers::served_cell_item_info{};
    cell_b_info.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
    cell_b_info.pci = 7;
    cell_a_cgi      = nr_cell_global_id_t{cell_a_info.plmn_id, cell_a_info.nci};
    cell_b_cgi      = nr_cell_global_id_t{cell_b_info.plmn_id, cell_b_info.nci};
  }

  /// Connect a new DU and run the F1 setup manually, returning the F1 Setup Response for inspection.
  /// Returns std::nullopt on any failure.
  std::optional<asn1::f1ap::f1_setup_resp_s> connect_du_and_run_f1_setup(unsigned& du_idx_out)
  {
    auto ret = connect_new_du();
    if (!ret.has_value()) {
      return std::nullopt;
    }
    du_idx_out = ret.value();

    get_du(du_idx_out)
        .push_ul_pdu(test_helpers::generate_f1_setup_request(int_to_gnb_du_id(0x11), {cell_a_info, cell_b_info}));

    f1ap_message resp;
    if (!wait_for_f1ap_tx_pdu(du_idx_out, resp)) {
      return std::nullopt;
    }
    if (resp.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types::successful_outcome ||
        resp.pdu.successful_outcome().proc_code != ASN1_F1AP_ID_F1_SETUP) {
      return std::nullopt;
    }
    return resp.pdu.successful_outcome().value.f1_setup_resp();
  }

  /// Collect the NCIs in the response's Cells to be Activated List.
  static std::vector<uint64_t> activated_ncis(const asn1::f1ap::f1_setup_resp_s& resp)
  {
    std::vector<uint64_t> ncis;
    if (!resp->cells_to_be_activ_list_present) {
      return ncis;
    }
    for (const auto& item : resp->cells_to_be_activ_list) {
      ncis.push_back(item->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.to_number());
    }
    return ncis;
  }

  /// Pop the next F1AP Tx PDU and return it if it is a gNB-CU Configuration Update.
  bool
  pop_cu_cfg_upd(unsigned du_idx, f1ap_message& out, std::chrono::milliseconds timeout = std::chrono::milliseconds{500})
  {
    if (!wait_for_f1ap_tx_pdu(du_idx, out, timeout)) {
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

  /// Run the full graceful lock of a cell (bar update ack, deactivation update ack) via deactivate_cell.
  void lock_cell(unsigned du_idx, const nr_cell_global_id_t& cgi)
  {
    cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

    async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.deactivate_cell(cgi);
    lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

    // Stage 1: bar update.
    f1ap_message bar_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(du_idx, bar_upd)) << "no bar update emitted by the lock";
    ASSERT_TRUE(bar_upd.pdu.init_msg().value.gnb_cu_cfg_upd()->cells_to_be_barred_list_present);
    get_du(du_idx).push_ul_pdu(make_ack_for(bar_upd));

    // Stage 3: deactivation update (no UEs attached in these tests, so stage 2 is empty).
    f1ap_message deact_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(du_idx, deact_upd)) << "no deactivation update emitted by the lock";
    ASSERT_TRUE(deact_upd.pdu.init_msg().value.gnb_cu_cfg_upd()->cells_to_be_deactiv_list_present);
    get_du(du_idx).push_ul_pdu(make_ack_for(deact_upd));

    ASSERT_TRUE(wait_for_task_result(launcher).success);
  }

  test_helpers::served_cell_item_info cell_a_info;
  test_helpers::served_cell_item_info cell_b_info;
  nr_cell_global_id_t                 cell_a_cgi;
  nr_cell_global_id_t                 cell_b_cgi;
};

/// Fixture with no declared logical cells (all cells learned dynamically).
class cu_cp_logical_cell_test : public cu_cp_logical_cell_test_base, public ::testing::Test
{
public:
  cu_cp_logical_cell_test() : cu_cp_logical_cell_test_base(cu_cp_test_env_params{}) {}
};

/// Fixture with cell B declared administratively locked in the CU-CP configuration.
class cu_cp_declared_locked_cell_test : public cu_cp_logical_cell_test_base, public ::testing::Test
{
public:
  cu_cp_declared_locked_cell_test() :
    cu_cp_logical_cell_test_base([]() {
      cu_cp_test_env_params env_params{};
      env_params.logical_cells = {ocucp::cu_cp_logical_cell_config{
          nr_cell_identity::create(gnb_id_t{411, 22}, 1).value(), /* admin_locked = */ true, /* barred = */ false}};
      return env_params;
    }())
  {
  }
};

/// Fixture with cell A declared barred (but unlocked) in the CU-CP configuration.
class cu_cp_declared_barred_cell_test : public cu_cp_logical_cell_test_base, public ::testing::Test
{
public:
  cu_cp_declared_barred_cell_test() :
    cu_cp_logical_cell_test_base([]() {
      cu_cp_test_env_params env_params{};
      env_params.logical_cells = {ocucp::cu_cp_logical_cell_config{
          nr_cell_identity::create(gnb_id_t{411, 22}, 0).value(), /* admin_locked = */ false, /* barred = */ true}};
      return env_params;
    }())
  {
  }
};

} // namespace

TEST_F(cu_cp_logical_cell_test, when_no_cells_declared_then_all_reported_cells_are_activated)
{
  // Regression guard: with no declared logical cells, dynamic cells default to unlocked and the F1 Setup
  // Response activates every reported cell, exactly as before the logical cell registry.
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());

  std::vector<uint64_t> ncis = activated_ncis(*resp);
  ASSERT_EQ(ncis.size(), 2U);
  EXPECT_NE(std::find(ncis.begin(), ncis.end(), cell_a_cgi.nci.value()), ncis.end());
  EXPECT_NE(std::find(ncis.begin(), ncis.end(), cell_b_cgi.nci.value()), ncis.end());
}

TEST_F(cu_cp_declared_locked_cell_test, when_cell_declared_locked_then_f1_setup_omits_it_from_activation)
{
  // The cell declared locked in configuration must not appear in the F1 Setup Response's Cells to be
  // Activated List; the other cell must.
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());

  std::vector<uint64_t> ncis = activated_ncis(*resp);
  ASSERT_EQ(ncis.size(), 1U) << "exactly one of the two reported cells should be activated";
  EXPECT_EQ(ncis[0], cell_a_cgi.nci.value());
}

TEST_F(cu_cp_declared_locked_cell_test, when_declared_locked_cell_is_unlocked_then_it_activates)
{
  // Pre-provisioned locked cell: boot locked, then unlock by command.
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());
  ASSERT_EQ(activated_ncis(*resp).size(), 1U);

  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  async_task<cu_cp_cell_command_response>         resp_task = cell_cmd.activate_cell(cell_b_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(resp_task);

  f1ap_message activ_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(du_idx, activ_upd)) << "unlock did not emit an activation update";
  const auto& upd_ies = activ_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_activ_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_activ_list.size(), 1U);
  ASSERT_EQ(upd_ies->cells_to_be_activ_list[0]->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.to_number(),
            cell_b_cgi.nci.value());
  get_du(du_idx).push_ul_pdu(make_ack_for(activ_upd));

  EXPECT_TRUE(wait_for_task_result(launcher).success);
}

TEST_F(cu_cp_logical_cell_test, when_locked_cell_du_reconnects_then_it_stays_locked)
{
  // The marquee persistence test: an operator lock must survive a DU restart. Today's DU records are erased
  // with the DU; the logical cell keeps the intent and re-applies it at the next F1 setup.
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());
  ASSERT_EQ(activated_ncis(*resp).size(), 2U);

  // Operator locks cell B (graceful stop: bar + deactivate).
  ASSERT_NO_FATAL_FAILURE(lock_cell(du_idx, cell_b_cgi));

  // The DU goes away and comes back.
  ASSERT_TRUE(drop_du_connection(du_idx));
  unsigned new_du_idx = 0;
  auto     resp2      = connect_du_and_run_f1_setup(new_du_idx);
  ASSERT_TRUE(resp2.has_value());

  // The locked cell must not be reactivated by the new F1 setup.
  std::vector<uint64_t> ncis = activated_ncis(*resp2);
  ASSERT_EQ(ncis.size(), 1U) << "the operator lock was lost across the DU restart";
  EXPECT_EQ(ncis[0], cell_a_cgi.nci.value());
}

TEST_F(cu_cp_declared_barred_cell_test, when_cell_declared_barred_then_bar_update_follows_f1_setup)
{
  // A barred (but unlocked) declared cell is activated at F1 setup and then barred right after it: the
  // CU-CP re-applies the barred intent with a gNB-CU Configuration Update carrying the Cells to be Barred
  // List.
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());
  ASSERT_EQ(activated_ncis(*resp).size(), 2U) << "a barred cell is activated normally";

  f1ap_message bar_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(du_idx, bar_upd)) << "no bar update followed the F1 setup of a barred cell";
  const auto& upd_ies = bar_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
  ASSERT_TRUE(upd_ies->cells_to_be_barred_list_present);
  ASSERT_EQ(upd_ies->cells_to_be_barred_list.size(), 1U);
  const auto& barred_item = upd_ies->cells_to_be_barred_list[0]->cells_to_be_barred_item();
  ASSERT_EQ(barred_item.nr_cgi.nr_cell_id.to_number(), cell_a_cgi.nci.value());
  ASSERT_EQ(barred_item.cell_barred.value, asn1::f1ap::cell_barred_opts::barred);
  get_du(du_idx).push_ul_pdu(make_ack_for(bar_upd));
}

TEST_F(cu_cp_logical_cell_test, when_bar_cell_command_then_f1ap_carries_barred_list)
{
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());

  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  // Bar the active cell A.
  {
    async_task<cu_cp_cell_command_response>         bar_task = cell_cmd.bar_cell(cell_a_cgi, true);
    lazy_task_launcher<cu_cp_cell_command_response> launcher(bar_task);

    f1ap_message bar_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(du_idx, bar_upd));
    const auto& upd_ies = bar_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(upd_ies->cells_to_be_barred_list_present);
    ASSERT_EQ(upd_ies->cells_to_be_barred_list[0]->cells_to_be_barred_item().cell_barred.value,
              asn1::f1ap::cell_barred_opts::barred);
    get_du(du_idx).push_ul_pdu(make_ack_for(bar_upd));

    ASSERT_TRUE(wait_for_task_result(launcher).success);
  }

  // Unbar it again: the update must carry cellBarred=notBarred.
  {
    async_task<cu_cp_cell_command_response>         unbar_task = cell_cmd.bar_cell(cell_a_cgi, false);
    lazy_task_launcher<cu_cp_cell_command_response> launcher(unbar_task);

    f1ap_message unbar_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(du_idx, unbar_upd));
    const auto& upd_ies = unbar_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(upd_ies->cells_to_be_barred_list_present);
    ASSERT_EQ(upd_ies->cells_to_be_barred_list[0]->cells_to_be_barred_item().cell_barred.value,
              asn1::f1ap::cell_barred_opts::not_barred);
    get_du(du_idx).push_ul_pdu(make_ack_for(unbar_upd));

    ASSERT_TRUE(wait_for_task_result(launcher).success);
  }
}

TEST_F(cu_cp_logical_cell_test, when_amf_reconnects_then_locked_cell_stays_locked_and_barred_cell_is_rebarred)
{
  // AMF loss deactivates the cells; AMF reconnection reactivates them. The reactivation must skip the
  // admin-locked cell (a fault-recovery activation must not override an operator lock) and re-apply the
  // barred intent to the reactivated cell (the DU restores its configured cellBarred on cell restart).
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());
  ASSERT_EQ(activated_ncis(*resp).size(), 2U);

  // Operator locks cell B and bars cell A (which stays active).
  ASSERT_NO_FATAL_FAILURE(lock_cell(du_idx, cell_b_cgi));
  {
    cu_cp_cell_command_handler&             cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();
    async_task<cu_cp_cell_command_response> bar_task = cell_cmd.bar_cell(cell_a_cgi, true);
    lazy_task_launcher<cu_cp_cell_command_response> launcher(bar_task);
    f1ap_message                                    bar_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(du_idx, bar_upd));
    get_du(du_idx).push_ul_pdu(make_ack_for(bar_upd));
    ASSERT_TRUE(wait_for_task_result(launcher).success);
  }

  // AMF loss: the CU-CP deactivates the still-served cell A; ack the deactivation update.
  ASSERT_TRUE(drop_amf_connection(0));
  {
    f1ap_message deact_upd;
    ASSERT_TRUE(pop_cu_cfg_upd(du_idx, deact_upd)) << "AMF loss did not deactivate the served cell";
    ASSERT_TRUE(deact_upd.pdu.init_msg().value.gnb_cu_cfg_upd()->cells_to_be_deactiv_list_present);
    get_du(du_idx).push_ul_pdu(make_ack_for(deact_upd));
  }

  // AMF reconnects: the activation update must carry only cell A (cell B stays locked).
  ASSERT_TRUE(reconnect_amf(0)) << "AMF did not reconnect within expected time";
  f1ap_message activ_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(du_idx, activ_upd, std::chrono::milliseconds{1000}))
      << "no activation update after AMF reconnection";
  {
    const auto& upd_ies = activ_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(upd_ies->cells_to_be_activ_list_present);
    ASSERT_EQ(upd_ies->cells_to_be_activ_list.size(), 1U) << "the locked cell must not be reactivated by AMF recovery";
    ASSERT_EQ(upd_ies->cells_to_be_activ_list[0]->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.to_number(),
              cell_a_cgi.nci.value());
  }
  get_du(du_idx).push_ul_pdu(make_ack_for(activ_upd));

  // The barred intent of cell A is re-applied right after the reactivation.
  f1ap_message rebar_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(du_idx, rebar_upd, std::chrono::milliseconds{1000}))
      << "barred intent was not re-applied after AMF reconnection";
  {
    const auto& upd_ies = rebar_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(upd_ies->cells_to_be_barred_list_present);
    ASSERT_EQ(upd_ies->cells_to_be_barred_list.size(), 1U);
    const auto& barred_item = upd_ies->cells_to_be_barred_list[0]->cells_to_be_barred_item();
    ASSERT_EQ(barred_item.nr_cgi.nr_cell_id.to_number(), cell_a_cgi.nci.value());
    ASSERT_EQ(barred_item.cell_barred.value, asn1::f1ap::cell_barred_opts::barred);
  }
  get_du(du_idx).push_ul_pdu(make_ack_for(rebar_upd));
}

TEST_F(cu_cp_logical_cell_test, when_locked_cell_is_barred_then_bar_follows_the_unlock)
{
  // Barring a dormant (locked) cell only records intent: no F1AP goes out. The bar is applied right after
  // the cell is unlocked, and the activation update restores the PLMNs parked at deactivation.
  unsigned du_idx = 0;
  auto     resp   = connect_du_and_run_f1_setup(du_idx);
  ASSERT_TRUE(resp.has_value());

  ASSERT_NO_FATAL_FAILURE(lock_cell(du_idx, cell_b_cgi));

  cu_cp_cell_command_handler& cell_cmd = get_cu_cp().get_command_handler().get_cell_command_handler();

  // Bar the dormant cell: completes synchronously with success and emits no F1AP.
  {
    async_task<cu_cp_cell_command_response>         bar_task = cell_cmd.bar_cell(cell_b_cgi, true);
    lazy_task_launcher<cu_cp_cell_command_response> launcher(bar_task);
    ASSERT_TRUE(launcher.ready()) << "barring a dormant cell should complete synchronously";
    ASSERT_TRUE(launcher.result.value().success);

    f1ap_message unexpected_pdu;
    ASSERT_FALSE(pop_cu_cfg_upd(du_idx, unexpected_pdu, std::chrono::milliseconds{50}))
        << "barring a dormant cell must not emit F1AP";
  }

  // Unlock the cell: activation update first (with the parked PLMNs restored), then the bar update.
  async_task<cu_cp_cell_command_response>         unlock_task = cell_cmd.activate_cell(cell_b_cgi);
  lazy_task_launcher<cu_cp_cell_command_response> launcher(unlock_task);

  f1ap_message activ_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(du_idx, activ_upd)) << "unlock did not emit an activation update";
  {
    const auto& upd_ies = activ_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(upd_ies->cells_to_be_activ_list_present);
    const auto& activ_item = upd_ies->cells_to_be_activ_list[0]->cells_to_be_activ_list_item();
    ASSERT_EQ(activ_item.nr_cgi.nr_cell_id.to_number(), cell_b_cgi.nci.value());
    // The PLMNs parked when the cell was deactivated are restored by the activation.
    ASSERT_TRUE(activ_item.ie_exts_present && activ_item.ie_exts.available_plmn_list_present)
        << "the unlock must restore the PLMNs parked at deactivation";
  }
  get_du(du_idx).push_ul_pdu(make_ack_for(activ_upd));

  f1ap_message bar_upd;
  ASSERT_TRUE(pop_cu_cfg_upd(du_idx, bar_upd)) << "the barred intent was not applied after the unlock";
  {
    const auto& upd_ies = bar_upd.pdu.init_msg().value.gnb_cu_cfg_upd();
    ASSERT_TRUE(upd_ies->cells_to_be_barred_list_present);
    ASSERT_EQ(upd_ies->cells_to_be_barred_list[0]->cells_to_be_barred_item().cell_barred.value,
              asn1::f1ap::cell_barred_opts::barred);
  }
  get_du(du_idx).push_ul_pdu(make_ack_for(bar_upd));

  EXPECT_TRUE(wait_for_task_result(launcher).success);
}
