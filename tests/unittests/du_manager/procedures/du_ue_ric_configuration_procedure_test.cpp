// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_manager_procedure_test_helpers.h"
#include "lib/du/du_high/du_manager/procedures/du_ue_ric_configuration_procedure.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class du_ue_ric_config_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  du_ue_ric_config_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
    test_ue = &create_ue(to_du_ue_index(test_rng::uniform_int<unsigned>(0, MAX_DU_UE_INDEX)));
  }

  void start_procedure(const du_mac_sched_control_config& request)
  {
    proc = launch_async<du_ue_ric_configuration_procedure>(request, ue_mng, params);
    proc_launcher.emplace(proc);
  }

  /// Sets up a DRB with the given S-NSSAI in the test UE, so that RRM Policy Members can be matched against it.
  void setup_ue_drb(const s_nssai_t& s_nssai)
  {
    f1ap_ue_context_update_request req = create_f1ap_ue_context_update_request(test_ue->ue_index, {}, {drb_id_t::drb1});
    req.drbs_to_setup[0].qos_info.s_nssai = s_nssai;
    configure_ue(req);
    mac.last_ue_reconf_msg.reset();
  }

  std::optional<du_mac_sched_control_config_response> proc_result()
  {
    return proc_launcher->ready() ? std::optional<du_mac_sched_control_config_response>{proc_launcher->get()}
                                  : std::nullopt;
  }

  du_ue*                                                                  test_ue = nullptr;
  async_task<du_mac_sched_control_config_response>                        proc;
  std::optional<lazy_task_launcher<du_mac_sched_control_config_response>> proc_launcher;
};

TEST_F(du_ue_ric_config_tester,
       when_new_ric_config_is_started_then_du_manager_starts_mac_config_and_waits_for_mac_response)
{
  du_mac_sched_control_config config_req;
  config_req.ue_id = test_ue->f1ap_ue_id;
  rrm_policy_ratio_group pol;
  pol.maximum_ratio   = 50;
  pol.minimum_ratio   = 5;
  pol.dedicated_ratio = 100;
  config_req.rrm_policy_ratio_list.push_back(pol);

  unsigned nof_prbs         = get_max_Nprb(params.ran.cells[0].ran.dl_carrier.carrier_bw,
                                   params.ran.cells[0].ran.dl_cfg_common.init_dl_bwp.generic_params.scs,
                                   band_helper::get_freq_range(params.ran.cells[0].ran.dl_carrier.band));
  int      expected_min_prb = static_cast<int>((1.0 * pol.minimum_ratio.value() / 100) * nof_prbs);
  int      expected_max_prb = static_cast<int>((1.0 * pol.maximum_ratio.value() / 100) * nof_prbs);

  start_procedure(config_req);

  ASSERT_TRUE(mac.last_ue_reconf_msg.has_value()) << "MAC should have received new configuration";
  ASSERT_EQ(mac.last_ue_reconf_msg->ue_index, test_ue->ue_index);
  ASSERT_TRUE(mac.last_ue_reconf_msg->sched_cfg.res_alloc_cfg.has_value());
  prb_interval expected_prbs{expected_min_prb, expected_max_prb};
  ASSERT_EQ(mac.last_ue_reconf_msg->sched_cfg.res_alloc_cfg->pdsch_grant_size_limits, expected_prbs);
  ASSERT_EQ(mac.last_ue_reconf_msg->sched_cfg.res_alloc_cfg->pusch_grant_size_limits, expected_prbs);
  ASSERT_FALSE(mac.last_ue_reconf_msg->sched_cfg.cells.has_value()) << "Cells should not have been configured";
  ASSERT_FALSE(mac.last_ue_reconf_msg->sched_cfg.lc_config_list.has_value())
      << "Logical channels should not have been configured";
  ASSERT_FALSE(proc_result().has_value()) << "The procedure should wait for MAC response";
}

TEST_F(du_ue_ric_config_tester, when_mac_finished_configuration_then_procedure_finishes)
{
  du_mac_sched_control_config config_req;
  config_req.ue_id = test_ue->f1ap_ue_id;
  rrm_policy_ratio_group pol;
  config_req.rrm_policy_ratio_list.push_back(pol);
  start_procedure(config_req);

  ASSERT_FALSE(proc_result().has_value()) << "The procedure should wait for MAC response";
  mac.wait_ue_reconf.result = mac_ue_reconfiguration_response{test_ue->ue_index, true};
  mac.wait_ue_reconf.ready_ev.set();
  ASSERT_TRUE(proc_result().has_value()) << "The procedure should have finished after receiving MAC response";
  ASSERT_TRUE(proc_result()->max_prb_alloc_result);
  ASSERT_TRUE(proc_result()->min_prb_alloc_result);
}

TEST_F(du_ue_ric_config_tester,
       when_new_ric_config_is_started_but_another_ue_procedure_is_running_then_ric_config_waits)
{
  // Schedule a dummy procedure in the UE task loop.
  manual_event_flag previous_event_complete;
  ue_mng.schedule_async_task(test_ue->ue_index,
                             launch_async([&previous_event_complete](coro_context<async_task<void>>& ctx) {
                               CORO_BEGIN(ctx);
                               CORO_AWAIT(previous_event_complete);
                               CORO_RETURN();
                             }));

  // Start RIC UE config.
  du_mac_sched_control_config config_req;
  config_req.ue_id = test_ue->f1ap_ue_id;
  rrm_policy_ratio_group pol;
  config_req.rrm_policy_ratio_list.push_back(pol);
  start_procedure(config_req);

  // Status: RIC config procedure is waiting for previous procedure.
  ASSERT_FALSE(mac.last_ue_reconf_msg.has_value())
      << "RIC configuration should not start until previous procedure finishes";

  // Previous procedure finishes.
  previous_event_complete.set();
  ASSERT_TRUE(mac.last_ue_reconf_msg.has_value())
      << "RIC configuration should not start until previous procedure finishes";
  ASSERT_FALSE(proc_result().has_value()) << "The procedure should wait for MAC response";

  // MAC finishes configuration and RIC procedure finishes.
  mac.wait_ue_reconf.result = mac_ue_reconfiguration_response{test_ue->ue_index, true};
  mac.wait_ue_reconf.ready_ev.set();
  ASSERT_TRUE(proc_result().has_value()) << "The procedure should have finished after receiving MAC response";
}

TEST_F(du_ue_ric_config_tester, when_rrm_policy_member_matches_ue_slice_then_mac_is_configured)
{
  // Give the UE a DRB, so that the RRM Policy Member has a slice of the UE to be matched against.
  const s_nssai_t ue_slice{slice_service_type{1}, slice_differentiator::create(1).value()};
  setup_ue_drb(ue_slice);

  // Start RIC UE config for the PLMN of the serving cell of the UE and the S-NSSAI of its DRB.
  du_mac_sched_control_config config_req;
  config_req.ue_id = test_ue->f1ap_ue_id;
  rrm_policy_ratio_group pol;
  pol.policy_members_list.push_back(rrm_policy_member{test_ue->nr_cgi.plmn_id, ue_slice});
  config_req.rrm_policy_ratio_list.push_back(pol);

  start_procedure(config_req);

  // Status: the RRM Policy Member matches, so the request is applied and acked.
  ASSERT_TRUE(mac.last_ue_reconf_msg.has_value()) << "MAC should have received new configuration";
  ASSERT_TRUE(proc_result().has_value());
  ASSERT_TRUE(proc_result()->min_prb_alloc_result);
  ASSERT_TRUE(proc_result()->max_prb_alloc_result);
}

TEST_F(du_ue_ric_config_tester, when_rrm_policy_member_has_other_s_nssai_then_ric_config_is_rejected)
{
  // Give the UE a DRB, so that the RRM Policy Member has a slice of the UE to be matched against.
  setup_ue_drb(s_nssai_t{slice_service_type{1}, slice_differentiator::create(1).value()});

  // Start RIC UE config for the PLMN of the serving cell of the UE, but for a S-NSSAI of no DRB of the UE.
  du_mac_sched_control_config config_req;
  config_req.ue_id = test_ue->f1ap_ue_id;
  rrm_policy_ratio_group pol;
  const s_nssai_t        other_slice{slice_service_type{1}, slice_differentiator::create(2).value()};
  pol.policy_members_list.push_back(rrm_policy_member{test_ue->nr_cgi.plmn_id, other_slice});
  config_req.rrm_policy_ratio_list.push_back(pol);

  start_procedure(config_req);

  // Status: the S-NSSAI does not match, so the request is nacked without reaching the MAC.
  ASSERT_FALSE(mac.last_ue_reconf_msg.has_value()) << "MAC should not be configured for a slice not used by the UE";
  ASSERT_TRUE(proc_result().has_value());
  ASSERT_FALSE(proc_result()->min_prb_alloc_result);
  ASSERT_FALSE(proc_result()->max_prb_alloc_result);
}

TEST_F(du_ue_ric_config_tester, when_rrm_policy_member_has_other_plmn_then_ric_config_is_rejected)
{
  // Give the UE a DRB, so that the RRM Policy Member has a slice of the UE to be matched against.
  const s_nssai_t ue_slice{slice_service_type{1}, slice_differentiator::create(1).value()};
  setup_ue_drb(ue_slice);

  // Start RIC UE config for the S-NSSAI of the DRB of the UE, but for a PLMN of another network.
  du_mac_sched_control_config config_req;
  config_req.ue_id = test_ue->f1ap_ue_id;
  rrm_policy_ratio_group pol;
  pol.policy_members_list.push_back(rrm_policy_member{plmn_identity::parse("00102").value(), ue_slice});
  config_req.rrm_policy_ratio_list.push_back(pol);

  start_procedure(config_req);

  // Status: the PLMN does not match, so the request is nacked even though the S-NSSAI is in use by the UE.
  ASSERT_FALSE(mac.last_ue_reconf_msg.has_value()) << "MAC should not be configured for a PLMN not used by the UE";
  ASSERT_TRUE(proc_result().has_value());
  ASSERT_FALSE(proc_result()->min_prb_alloc_result);
  ASSERT_FALSE(proc_result()->max_prb_alloc_result);
}
