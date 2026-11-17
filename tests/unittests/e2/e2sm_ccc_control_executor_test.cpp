// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e2/e2sm/e2sm_ccc/e2sm_ccc_control_action_du_executor.h"
#include "lib/e2/e2sm/e2sm_ccc/e2sm_ccc_control_service_impl.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace asn1::e2sm_ccc;
using namespace ocudu;

namespace {

/// Records what the executor asked of the DU, so that a request that is turned away can be told from one that is
/// applied with values the RIC never sent.
class recording_du_configurator : public odu::du_configurator
{
public:
  async_task<odu::du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(const odu::du_mac_sched_control_config& reconf) override
  {
    return launch_async([](coro_context<async_task<odu::du_mac_sched_control_config_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(odu::du_mac_sched_control_config_response{true, true, true});
    });
  }
  odu::du_param_config_response handle_sync_operator_config(const odu::du_param_config_request& req) override
  {
    return odu::du_param_config_response{};
  }
  async_task<odu::du_param_config_response> handle_operator_config(const odu::du_param_config_request& req,
                                                                   task_executor&                      exec) override
  {
    applied.push_back(req);
    return launch_async([](coro_context<async_task<odu::du_param_config_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(odu::du_param_config_response{true});
    });
  }
  void handle_ntn_param_update(odu::du_ntn_param_update_request req) override {}

  std::vector<odu::du_param_config_request> applied;
};

/// Fills a structure with an O-RRMPolicyRatio that the executor accepts, which each test then varies.
void set_valid_policy(cfg_structure_write_s& structure)
{
  structure.ran_cfg_structure_name.from_string("O-RRMPolicyRatio");

  auto& policy                        = structure.new_values_of_attributes.ran_cfg_structure.set_o_rrm_policy_ratio();
  policy.res_type_present             = true;
  policy.res_type                     = res_type_opts::prb_dl;
  policy.rrm_policy_min_ratio_present = true;
  policy.rrm_policy_min_ratio         = 10;
  policy.rrm_policy_max_ratio_present = true;
  policy.rrm_policy_max_ratio         = 75;
  policy.rrm_policy_ded_ratio_present = true;
  policy.rrm_policy_ded_ratio         = 52;
  policy.rrm_policy_member_list.resize(1);
  auto& member               = policy.rrm_policy_member_list.back();
  member.plmn_id_present     = true;
  member.plmn_id.mcc_present = true;
  member.plmn_id.mcc.from_string("001");
  member.plmn_id.mnc_present = true;
  member.plmn_id.mnc.from_string("01");
}

/// Builds a control request of one cell holding the given number of O-RRMPolicyRatio structures.
e2sm_ric_control_request make_request(unsigned nof_structures)
{
  ric_ctrl_hdr_s hdr;
  hdr.ctrl_hdr_format.ctrl_hdr_format1().ric_style_type = 2;

  ric_ctrl_msg_s msg;
  auto&          format2 = msg.ctrl_msg_format.set_ctrl_msg_format2();
  format2.list_of_cells_ctrl.resize(1);
  auto& cell   = format2.list_of_cells_ctrl.back();
  auto& nr_cgi = cell.cell_global_id.set_nr_cgi();
  nr_cgi.nr_cell_id.from_number(6733824);
  nr_cgi.plmn_id.mcc_present = true;
  nr_cgi.plmn_id.mcc.from_string("001");
  nr_cgi.plmn_id.mnc_present = true;
  nr_cgi.plmn_id.mnc.from_string("01");

  cell.list_of_cfg_structures.resize(nof_structures);
  for (unsigned i = 0; i != nof_structures; ++i) {
    set_valid_policy(cell.list_of_cfg_structures[i]);
  }

  e2sm_ric_control_request req = {};
  req.service_model            = e2sm_service_model_t::CCC;
  req.request_ctrl_hdr         = hdr;
  req.request_ctrl_msg         = msg;
  return req;
}

/// The list of cells that the request carries.
auto& cells_of(e2sm_ric_control_request& req)
{
  auto& msg = std::get<ric_ctrl_msg_s>(req.request_ctrl_msg);
  return msg.ctrl_msg_format.ctrl_msg_format2().list_of_cells_ctrl;
}

/// The structure of the given index of the one cell the request carries.
cfg_structure_write_s& structure_of(e2sm_ric_control_request& req, unsigned index)
{
  return cells_of(req)[0].list_of_cfg_structures[index];
}

/// Builds a control request of two cells, where the cell of the given index holds no structure and the other holds
/// one O-RRMPolicyRatio.
e2sm_ric_control_request make_two_cell_request_with_an_empty_cell(unsigned empty_cell_index)
{
  e2sm_ric_control_request req   = make_request(1);
  auto&                    cells = cells_of(req);
  cells.resize(2);
  cells[1] = cells[0];
  cells[empty_cell_index].list_of_cfg_structures.resize(0);
  return req;
}

class e2sm_ccc_control_executor_test : public ::testing::Test
{
protected:
  recording_du_configurator                    du_configurator;
  manual_task_worker                           worker{16};
  e2sm_ccc_control_o_rrm_policy_ratio_executor executor{du_configurator, worker};
};

class e2sm_ccc_control_service_style_2_test : public ::testing::Test
{
protected:
  e2sm_ccc_control_service_style_2_test()
  {
    service.add_e2sm_rc_control_action_executor(
        std::make_unique<e2sm_ccc_control_o_rrm_policy_ratio_executor>(du_configurator, worker));
  }

  /// Runs the request through the service and checks that it fails without reaching the DU.
  void assert_request_fails_without_configuring_the_du(const e2sm_ric_control_request& req)
  {
    ASSERT_FALSE(service.control_request_supported(req));
    auto                                          task = service.execute_control_request(req);
    lazy_task_launcher<e2sm_ric_control_response> launcher(task);
    ASSERT_TRUE(task.ready());
    ASSERT_FALSE(task.get().success);
    ASSERT_TRUE(du_configurator.applied.empty());
  }

  recording_du_configurator        du_configurator;
  manual_task_worker               worker{16};
  e2sm_ccc_control_service_style_2 service;
};

} // namespace

TEST_F(e2sm_ccc_control_executor_test, a_request_of_one_sound_structure_is_supported)
{
  e2sm_ric_control_request req = make_request(1);
  ASSERT_TRUE(executor.ric_control_action_supported(req));
}

TEST_F(e2sm_ccc_control_executor_test, a_second_structure_whose_values_hold_another_alternative_is_rejected)
{
  // The first structure is sound and the second names O-RRMPolicyRatio while carrying the values of another
  // structure, which a check of the first structure alone would let through.
  e2sm_ric_control_request req = make_request(2);
  structure_of(req, 1).new_values_of_attributes.ran_cfg_structure.set_o_nr_cell_du();

  ASSERT_FALSE(executor.ric_control_action_supported(req));
}

TEST_F(e2sm_ccc_control_executor_test, a_second_structure_without_a_resource_type_is_rejected)
{
  e2sm_ric_control_request req = make_request(2);
  structure_of(req, 1).new_values_of_attributes.ran_cfg_structure.o_rrm_policy_ratio().res_type_present = false;

  ASSERT_FALSE(executor.ric_control_action_supported(req));
}

TEST_F(e2sm_ccc_control_executor_test, a_structure_of_another_name_leaves_this_executor_unsupported)
{
  e2sm_ric_control_request req = make_request(1);
  structure_of(req, 0).ran_cfg_structure_name.from_string("O-NrCellDu");

  ASSERT_FALSE(executor.ric_control_action_supported(req));
}

// The decoder refuses empty lists, so these checks guard a request that reaches the executor without it.
TEST_F(e2sm_ccc_control_executor_test, a_request_without_cells_is_rejected)
{
  e2sm_ric_control_request req = make_request(1);
  cells_of(req).resize(0);

  ASSERT_FALSE(executor.ric_control_action_supported(req));
}

TEST_F(e2sm_ccc_control_executor_test, a_first_cell_without_structures_is_rejected)
{
  ASSERT_FALSE(executor.ric_control_action_supported(make_two_cell_request_with_an_empty_cell(0)));
}

TEST_F(e2sm_ccc_control_executor_test, a_second_cell_without_structures_is_rejected)
{
  ASSERT_FALSE(executor.ric_control_action_supported(make_two_cell_request_with_an_empty_cell(1)));
}

TEST_F(e2sm_ccc_control_executor_test, a_first_structure_whose_values_hold_another_alternative_is_rejected)
{
  e2sm_ric_control_request req = make_request(1);
  structure_of(req, 0).new_values_of_attributes.ran_cfg_structure.set_o_nr_cell_du();

  ASSERT_FALSE(executor.ric_control_action_supported(req));
}

TEST_F(e2sm_ccc_control_service_style_2_test, a_request_of_one_sound_structure_is_supported)
{
  ASSERT_TRUE(service.control_request_supported(make_request(1)));
}

TEST_F(e2sm_ccc_control_service_style_2_test, a_request_without_cells_fails)
{
  e2sm_ric_control_request req = make_request(1);
  cells_of(req).resize(0);

  assert_request_fails_without_configuring_the_du(req);
}

TEST_F(e2sm_ccc_control_service_style_2_test, a_cell_without_structures_fails)
{
  assert_request_fails_without_configuring_the_du(make_request(0));
}

TEST_F(e2sm_ccc_control_service_style_2_test, a_first_cell_without_structures_fails)
{
  assert_request_fails_without_configuring_the_du(make_two_cell_request_with_an_empty_cell(0));
}

TEST_F(e2sm_ccc_control_service_style_2_test, a_second_cell_without_structures_fails)
{
  assert_request_fails_without_configuring_the_du(make_two_cell_request_with_an_empty_cell(1));
}

TEST_F(e2sm_ccc_control_executor_test, a_dedicated_ratio_of_zero_is_applied_rather_than_taken_as_absent)
{
  // Zero is a ratio the RIC may ask for, and a presence flag is what says whether it sent one.
  e2sm_ric_control_request req = make_request(1);
  structure_of(req, 0).new_values_of_attributes.ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_ded_ratio = 0;

  auto                                          task = executor.execute_ric_control_action(req);
  lazy_task_launcher<e2sm_ric_control_response> launcher(task);
  ASSERT_TRUE(task.ready());
  ASSERT_TRUE(task.get().success);
  ASSERT_EQ(du_configurator.applied.size(), 1);
  ASSERT_TRUE(du_configurator.applied[0].cells[0].rrm_policy_ratio_list[0].dedicated_ratio.has_value());
  ASSERT_EQ(du_configurator.applied[0].cells[0].rrm_policy_ratio_list[0].dedicated_ratio.value(), 0);
}

TEST_F(e2sm_ccc_control_executor_test, a_negative_ratio_is_not_applied_as_a_large_quota)
{
  // The destination holds a ratio unsigned, so a negative one would wrap into a quota the RIC never asked for.
  e2sm_ric_control_request req = make_request(1);
  structure_of(req, 0).new_values_of_attributes.ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_min_ratio = -1;

  auto                                          task = executor.execute_ric_control_action(req);
  lazy_task_launcher<e2sm_ric_control_response> launcher(task);
  ASSERT_TRUE(task.ready());
  ASSERT_FALSE(task.get().success);
  ASSERT_TRUE(du_configurator.applied.empty());
}

TEST_F(e2sm_ccc_control_executor_test, a_ratio_above_its_percentage_is_not_applied)
{
  e2sm_ric_control_request req = make_request(1);
  structure_of(req, 0).new_values_of_attributes.ran_cfg_structure.o_rrm_policy_ratio().rrm_policy_max_ratio = 101;

  auto                                          task = executor.execute_ric_control_action(req);
  lazy_task_launcher<e2sm_ric_control_response> launcher(task);
  ASSERT_TRUE(task.ready());
  ASSERT_FALSE(task.get().success);
  ASSERT_TRUE(du_configurator.applied.empty());
}
