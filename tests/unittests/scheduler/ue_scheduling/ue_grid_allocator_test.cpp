// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/logging/cell_metrics_handler.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/slicing/ran_slice_instance.h"
#include "lib/scheduler/srs/srs_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_context/ue.h"
#include "lib/scheduler/ue_context/ue_cell_repository.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/scheduler/scheduler_result_finder.h"
#include "tests/test_doubles/scheduler/scheduler_test_message_validators.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/duplex_mode.h"
#include "ocudu/ran/pdcch/search_space.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/support/rb_helper.h"
#include <gtest/gtest.h>

using namespace ocudu;

struct test_params {
  scheduler_expert_config sched_cfg;
  std::variant<search_space_configuration::common_dci_format, search_space_configuration::ue_specific_dci_format>
      ss2_dci_fmt = search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;
};

class ue_grid_allocator_test : public ::testing::TestWithParam<duplex_mode>
{
  static sched_cell_configuration_request_message
  make_cell_config_request(const cell_config_builder_params& builder_params, const test_params& params)
  {
    sched_cell_configuration_request_message req;
    req = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    req.ran.init_bwp.pdcch_cfg->search_spaces[0].set_non_ss0_monitored_dci_formats(params.ss2_dci_fmt);
    return req;
  }

protected:
  ue_grid_allocator_test(const test_params& params) :
    sched_cfg(params.sched_cfg),
    cfg_builder_params(cell_config_builder_profiles::create(GetParam())),
    cell_cfg(*[this, &params]() {
      const auto* cfg = cfg_mng.add_cell(make_cell_config_request(cfg_builder_params, params));
      ocudu_assert(cfg != nullptr, "Cell configuration failed");
      return cfg;
    }()),
    cell_ues(cell_cfg, nullptr),
    ues(sched_cfg.ue),
    slice_ues(ran_slice_id_t{0}, to_du_cell_index(0), ues),
    alloc(expert_cfg, ues, pdcch_alloc, uci_alloc, srs_alloc, res_grid, logger),
    current_slot(cfg_builder_params.scs_common, 0)
  {
    ues.register_cell(cell_ues);
    logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    // Initialize resource grid.
    slot_indication();
  }

  slot_point get_next_ul_slot(const slot_point starting_slot) const
  {
    slot_point next_slot = starting_slot + cfg_builder_params.min_k2;
    while (not cell_cfg.is_fully_ul_enabled(next_slot)) {
      ++next_slot;
    }
    return next_slot;
  }

  void slot_indication(const std::function<void()>& on_each_slot = []() {})
  {
    ++current_slot;
    logger.set_context(current_slot.sfn(), current_slot.slot_index());

    res_grid.slot_indication(current_slot);
    pdcch_alloc.slot_indication(current_slot);
    pucch_alloc.slot_indication(current_slot);
    uci_alloc.slot_indication(current_slot);
    ues.slot_indication(current_slot);

    // Prepare CRB bitmask that will be used to find available CRBs.
    const auto& init_dl_bwp = cell_cfg.params.dl_cfg_common.init_dl_bwp;
    const auto  prb_lims    = crb_to_prb(init_dl_bwp.generic_params.crbs,
                                     cell_cfg.expert_cfg.ue.pdsch_crb_limits & init_dl_bwp.generic_params.crbs);
    auto        used_prbs   = res_grid[0].dl_res_grid.used_prbs(init_dl_bwp.generic_params.scs,
                                                       init_dl_bwp.generic_params.crbs,
                                                       init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].symbols);
    if (not prb_lims.empty()) {
      used_prbs.fill(0, prb_lims.start());
      used_prbs.fill(prb_lims.stop(), used_prbs.size());
    }
    // Note: VRB-to-PRB interleaving is not supported in this test.
    used_dl_vrbs = used_prbs.convert_to<vrb_bitmap>();

    on_each_slot();

    alloc.post_process_results();

    // Log scheduler results.
    res_logger.on_scheduler_result(res_grid[0].result);
  }

  bool run_until(const std::function<void()>& to_run, unique_function<bool()> until, unsigned max_slot_count = 1000)
  {
    if (until()) {
      return true;
    }
    for (unsigned count = 0; count != max_slot_count; ++count) {
      slot_indication(to_run);
      if (until()) {
        return true;
      }
    }
    return false;
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
    ue_creation_req.ue_index = ue_index;
    ue_creation_req.crnti    = to_rnti(0x4601 + (unsigned)ue_index);
    for (lcid_t lcid : lcids_to_activate) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    // Set same dedicated PDCCH config.
    (*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg =
        cell_cfg.bwp_res[to_bwp_id(0)].dl().ded_pdcchs[0];

    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_creation_req)
  {
    auto ev = cfg_mng.add_ue(ue_creation_req);
    report_fatal_error_if_not(ev.valid(), "UE creation failed");
    ues.add_ue(ev.next_config(),
               {sched_config_helper::to_ue_creation_mode(ue_creation_req), ue_creation_req.ul_ccch_slot_rx});
    for (const auto& lc_cfg : *ue_creation_req.cfg.lc_config_list) {
      slice_ues.add_logical_channel(ues[ue_creation_req.ue_index], lc_cfg.lcid, lc_cfg.lc_group);
    }
    ev.notify_completion();
    return ues[ue_creation_req.ue_index];
  }

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    ues[ue_index].handle_dl_buffer_state_indication(lcid, bytes);
  }

  void allocate_dl_newtx_grant(const slice_ue&         user,
                               units::bytes            pending_bytes,
                               bool                    interleaving_enabled,
                               std::optional<unsigned> max_nof_rbs = std::nullopt)
  {
    const auto& init_dl_bwp = cell_cfg.params.dl_cfg_common.init_dl_bwp;
    auto        result =
        alloc.allocate_dl_grant(ue_newtx_dl_grant_request{user, current_slot, pending_bytes, interleaving_enabled});
    if (not result.has_value()) {
      return;
    }
    auto& builder = result.value();

    vrb_interval vrbs = builder.recommended_vrbs(used_dl_vrbs);

    // Compute the corresponding CRBs.
    // Note: VRB-to-PRB interleaving is not supported in this test.
    std::pair<crb_interval, crb_interval> crbs = {
        prb_to_crb(init_dl_bwp.generic_params.crbs, vrbs.convert_to<prb_interval>()), {}};

    builder.set_pdsch_params(vrbs, crbs, interleaving_enabled);
    used_dl_vrbs.fill(vrbs.start(), vrbs.stop());
  }

  void allocate_dl_retx_grant(const slice_ue& user, dl_harq_process_handle h_dl)
  {
    auto result = alloc.allocate_dl_grant(ue_retx_dl_grant_request{user, current_slot, h_dl, used_dl_vrbs});
    if (result.has_value()) {
      used_dl_vrbs.fill(result.value().vrbs.start(), result.value().vrbs.stop());
    }
  }

  alloc_status allocate_ul_newtx_grant(const slice_ue&         user,
                                       units::bytes            pending_bytes,
                                       std::optional<unsigned> max_nof_rbs = std::nullopt)
  {
    return allocate_ul_newtx_grant(get_next_ul_slot(current_slot), user, pending_bytes, max_nof_rbs);
  }

  alloc_status allocate_ul_newtx_grant(slot_point              pusch_slot,
                                       const slice_ue&         user,
                                       units::bytes            pending_bytes,
                                       std::optional<unsigned> max_nof_rbs = std::nullopt)
  {
    const auto& init_ul_bwp = cell_cfg.params.ul_cfg_common.init_ul_bwp;
    auto        result      = alloc.allocate_ul_grant(ue_newtx_ul_grant_request{
        user, pusch_slot, pending_bytes, ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP}});
    if (not result.has_value()) {
      return result.error();
    }
    auto& builder = result.value();

    // Note: VRB-to-PRB interleaving is not supported in this test.
    auto used_ul_vrbs = res_grid[pusch_slot]
                            .ul_res_grid
                            .used_prbs(init_ul_bwp.generic_params.scs,
                                       init_ul_bwp.generic_params.crbs,
                                       init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols)
                            .convert_to<vrb_bitmap>();
    vrb_interval vrbs = builder.recommended_vrbs(used_ul_vrbs, max_nof_rbs.value_or(MAX_NOF_PRBS));
    builder.set_pusch_params(vrbs);
    used_ul_vrbs.fill(vrbs.start(), vrbs.stop());
    return alloc_status::success;
  }

  scheduler_expert_config                 sched_cfg;
  scheduler_ue_expert_config              expert_cfg{sched_cfg.ue};
  sched_cfg_dummy_notifier                mac_notif;
  scheduler_ue_metrics_dummy_notifier     metrics_notif;
  scheduler_ue_metrics_dummy_configurator metrics_ue_handler;

  cell_config_builder_params cfg_builder_params;
  sched_config_manager       cfg_mng{scheduler_config{sched_cfg, mac_notif}};
  const cell_configuration&  cell_cfg;

  cell_resource_allocator res_grid{cell_cfg};

  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl pucch_alloc{cell_cfg, expert_cfg.max_pucchs_per_slot, expert_cfg.max_ul_grants_per_slot};
  uci_allocator_impl   uci_alloc{cell_cfg, pucch_alloc};
  srs_allocator_impl   srs_alloc{cell_cfg, std::nullopt};

  ocudulog::basic_logger& logger{ocudulog::fetch_basic_logger("SCHED")};
  scheduler_result_logger res_logger{false, cell_cfg.params.pci};

  ue_cell_repository      cell_ues;
  ue_repository           ues;
  slice_ue_repository     slice_ues;
  slice_rrm_policy_config rrm_policy;
  ran_slice_instance      slice_inst{ran_slice_id_t{0}, cell_cfg, rrm_policy, ues};
  ue_cell_grid_allocator  alloc;

  slot_point current_slot;
  vrb_bitmap used_dl_vrbs;
};

class ue_grid_allocator_css_test : public ue_grid_allocator_test
{
protected:
  ue_grid_allocator_css_test() :
    ue_grid_allocator_test(test_params{config_helpers::make_default_scheduler_expert_config(),
                                       search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true}})
  {
  }
};

TEST_P(ue_grid_allocator_css_test,
       when_ue_dedicated_ss_is_css_then_allocation_is_within_coreset_start_crb_and_coreset0_end_crb)
{
  static const units::bytes nof_bytes_to_schedule{40U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.cfg.cells->front().serv_cell_cfg.init_dl_bwp.pdcch_cfg =
      cell_cfg.bwp_res[to_bwp_id(0)].dl().ded_pdcchs[0];
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  ue& u                    = add_ue(ue_creation_req);

  const auto&        cs1_cfg  = u.ue_cfg_dedicated()->pcell_cfg().coreset(to_coreset_id(1)).cfg();
  const crb_interval cs1_crbs = get_coreset_crbs(cs1_cfg);
  const crb_interval crb_lims = {
      cs1_crbs.start(),
      cs1_crbs.start() + cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()};

  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  const auto& prb_alloc = res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().convert_to<prb_interval>();
  const auto  crb_alloc = prb_to_crb(cs1_crbs, prb_alloc);
  ASSERT_TRUE(crb_lims.contains(crb_alloc));
}

class ue_grid_allocator_default_cfg_test : public ue_grid_allocator_test
{
protected:
  ue_grid_allocator_default_cfg_test(
      const scheduler_expert_config& sched_cfg_ = config_helpers::make_default_scheduler_expert_config()) :
    ue_grid_allocator_test(test_params{sched_cfg_})
  {
  }
};

TEST_P(ue_grid_allocator_default_cfg_test, when_using_non_fallback_dci_format_use_mcs_table_set_in_pdsch_cfg)
{
  static const units::bytes nof_bytes_to_schedule{40U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  (*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = ocudu::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                       = to_du_ue_index(0);
  ue_creation_req.crnti                                                          = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#2 uses non-fallback DCI format hence the MCS table set in dedicated PDSCH configuration must be used.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.mcs_table, ocudu::pdsch_mcs_table::qam256);
}

TEST_P(ue_grid_allocator_default_cfg_test, allocates_pdsch_restricted_to_recommended_max_nof_rbs)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  static const units::bytes sched_bytes{2000U};
  const unsigned            max_nof_rbs_to_schedule = 10U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH corresponding to the grant.
  ASSERT_GE(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            max_nof_rbs_to_schedule);
}

TEST_P(ue_grid_allocator_default_cfg_test, allocates_pusch_restricted_to_recommended_max_nof_rbs)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  const units::bytes recommended_nof_bytes_to_schedule{2000U};
  const unsigned     max_nof_rbs_to_schedule = 10U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), max_nof_rbs_to_schedule);
}

TEST_P(ue_grid_allocator_default_cfg_test, does_not_allocate_pusch_with_all_remaining_rbs_if_its_a_sr_indication)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  ue& u1                   = add_ue(ue_creation_req);
  // Trigger a SR indication.
  u1.handle_sr_indication(current_slot);

  const crb_interval cell_crbs = {cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.start(),
                                  cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.stop()};

  ASSERT_TRUE(run_until([&]() { allocate_ul_newtx_grant(slice_ues[u1.ue_index], u1.pending_ul_newtx_bytes()); },
                        [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_LT(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), cell_crbs.length());
}

TEST_P(ue_grid_allocator_default_cfg_test, no_two_pdschs_are_allocated_in_same_slot_for_a_ue)
{
  static const units::bytes nof_bytes_to_schedule{400U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false);
        allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false);
      },
      [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  // Only one PDSCH per slot per UE.
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.size(), 1);
}

TEST_P(ue_grid_allocator_default_cfg_test, no_two_puschs_are_allocated_in_same_slot_for_a_ue)
{
  static const units::bytes nof_bytes_to_schedule{400U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule);
        allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule);
      },
      [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));

  // Only one PUSCH per slot per UE.
  ASSERT_EQ(res_grid[0].result.ul.puschs.size(), 1);
}

TEST_P(ue_grid_allocator_default_cfg_test, consecutive_puschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const units::bytes nof_bytes_to_schedule{400U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  // First PUSCH grant for the UE.
  slot_point pusch_slot;
  ASSERT_TRUE(run_until(
      [&]() {
        pusch_slot = get_next_ul_slot(current_slot);
        allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], nof_bytes_to_schedule);
      },
      [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));

  // Second PUSCH grant for the UE trying to allocate PUSCH in a slot previous to grant1.
  alloc_status result = alloc_status::invalid_params;
  ASSERT_FALSE(run_until(
      [&]() { result = allocate_ul_newtx_grant(pusch_slot - 1, slice_ues[u.ue_index], nof_bytes_to_schedule); },
      [&]() { return result == alloc_status::success; },
      1));
}

TEST_P(ue_grid_allocator_default_cfg_test, consecutive_pdschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const units::bytes nof_bytes_to_schedule{400U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  slot_point last_pdsch_slot = current_slot;

  // Second PDSCH grant in the same slot for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_GE(current_slot, last_pdsch_slot);
}

TEST_P(ue_grid_allocator_default_cfg_test,
       ack_slot_of_consecutive_pdschs_for_a_ue_must_be_greater_than_or_equal_to_last_ack_slot_allocated)
{
  static const units::bytes nof_bytes_to_schedule{400U};

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  slot_point last_pdsch_ack_slot = current_slot + find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants)->context.k1;

  // Second PDSCH grant in the same slot for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_GE(current_slot + find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants)->context.k1, last_pdsch_ack_slot);
}

TEST_P(ue_grid_allocator_default_cfg_test,
       successfully_allocated_pdsch_even_with_large_gap_to_last_pdsch_slot_allocated)
{
  static const units::bytes nof_bytes_to_schedule{8U};
  const unsigned            nof_slot_until_pdsch_is_allocated_threshold = SCHEDULER_MAX_K0;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  // Ensure current slot is the middle of 1024 SFNs. i.e. current slot=511.0
  while (current_slot.sfn() != NOF_SFNS / 2) {
    slot_indication();
  }

  // First PDSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  // Ensure next PDSCH to be allocated slot is after wrap around of 1024 SFNs (large gap to last allocated PDSCH slot)
  // and current slot value is less than last allocated PDSCH slot. e.g. next PDSCH to be allocated slot=SFN 2, slot 2
  // after wrap around of 1024 SFNs.
  for (unsigned i = 0; i < current_slot.nof_slots_per_hyper_system_frame() / 2 + current_slot.nof_slots_per_frame();
       ++i) {
    slot_indication();
  }

  // Next PDSCH grant to be allocated.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; },
                        nof_slot_until_pdsch_is_allocated_threshold));
}

TEST_P(ue_grid_allocator_default_cfg_test, successfully_allocates_pdsch_with_gbr_lc_prioritized_over_non_gbr_lc)
{
  OCUDU_TEST_REQUIREMENTS("DU-QOS-1");

  const lcg_id_t lcg_id              = uint_to_lcg_id(2);
  const lcid_t   gbr_bearer_lcid     = uint_to_lcid(6);
  const lcid_t   non_gbr_bearer_lcid = uint_to_lcid(5);

  // Add UE.
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  ue& u1                   = add_ue(ue_creation_req);

  // Reconfigure UE to include non-GBR bearer and GBR bearer.
  sched_ue_reconfiguration_message reconf_msg{
      .ue_index = ue_creation_req.ue_index, .crnti = ue_creation_req.crnti, .cfg = ue_creation_req.cfg};
  sched_ue_config_request& cfg_req = reconf_msg.cfg;
  cfg_req.lc_config_list.emplace();
  cfg_req.lc_config_list->resize(4);
  (*cfg_req.lc_config_list)[0]          = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);
  (*cfg_req.lc_config_list)[1]          = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1);
  (*cfg_req.lc_config_list)[2]          = config_helpers::create_default_logical_channel_config(non_gbr_bearer_lcid);
  (*cfg_req.lc_config_list)[2].lc_group = lcg_id;
  (*cfg_req.lc_config_list)[2].qos.emplace();
  (*cfg_req.lc_config_list)[2].qos->qos          = *get_5qi_to_qos_characteristics_mapping(uint_to_five_qi(9));
  (*cfg_req.lc_config_list)[2].qos->arp_priority = arp_prio_level_t::max();
  (*cfg_req.lc_config_list)[3] = config_helpers::create_default_logical_channel_config(gbr_bearer_lcid);
  // Put GBR bearer in a different LCG than non-GBR bearer.
  (*cfg_req.lc_config_list)[3].lc_group = uint_to_lcg_id(lcg_id - 1);
  (*cfg_req.lc_config_list)[3].qos.emplace();
  (*cfg_req.lc_config_list)[3].qos->qos          = *get_5qi_to_qos_characteristics_mapping(uint_to_five_qi(1));
  (*cfg_req.lc_config_list)[3].qos->arp_priority = arp_prio_level_t::max();
  (*cfg_req.lc_config_list)[3].qos->gbr_qos_info = gbr_qos_flow_information{128000, 128000, 128000, 128000};
  ue_config_update_event ev                      = cfg_mng.update_ue(reconf_msg);
  ues.reconfigure_ue(ev.next_config(), sched_ue_config_request::causes::other_rrc_proc);
  ues.ue_config_applied(ev.get_ue_index());

  // Add LCID to the bearers of the UE belonging to this slice.
  for (const auto& lc_cfg : *cfg_req.lc_config_list) {
    slice_ues.add_logical_channel(u1, lc_cfg.lcid, lc_cfg.lc_group);
  }

  // Push buffer state update to both bearers.
  push_dl_bs(u1.ue_index, gbr_bearer_lcid, 200);
  push_dl_bs(u1.ue_index, non_gbr_bearer_lcid, 1500);

  static const units::bytes sched_bytes{2000U};

  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, false); },
                        [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  const auto* ue_pdsch = find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants);
  ASSERT_TRUE(not ue_pdsch->tb_list.empty());
  ASSERT_TRUE(not ue_pdsch->tb_list.back().lc_chs_to_sched.empty());
  // TB info contains GBR LC channel first and then non-GBR LC channel.
  ASSERT_EQ(ue_pdsch->tb_list.back().lc_chs_to_sched.front().lcid, gbr_bearer_lcid);
}

TEST_P(ue_grid_allocator_default_cfg_test,
       successfully_allocated_pusch_even_with_large_gap_to_last_pusch_slot_allocated)
{
  static const units::bytes nof_bytes_to_schedule{400U};
  const unsigned            nof_slot_until_pusch_is_allocated_threshold = SCHEDULER_MAX_K2;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  const ue& u = add_ue(ue_creation_req);

  // Ensure current slot is the middle of 1024 SFNs. i.e. current slot=511.0
  while (current_slot.sfn() != NOF_SFNS / 2) {
    slot_indication();
  }

  // First PUSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule); },
                        [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul.puschs) != nullptr; }));

  // Ensure next PUSCH to be allocated slot is after wrap around of 1024 SFNs (large gap to last allocated PUSCH slot)
  // and current slot value is less than last allocated PUSCH slot. e.g. next PUSCH to be allocated slot=SFN 2, slot 2
  // after wrap around of 1024 SFNs.
  for (unsigned i = 0; i < current_slot.nof_slots_per_hyper_system_frame() / 2 + current_slot.nof_slots_per_frame();
       ++i) {
    slot_indication();
  }

  // Second PUSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule); },
                        [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul.puschs) != nullptr; },
                        nof_slot_until_pusch_is_allocated_threshold));
}

class ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester : public ue_grid_allocator_default_cfg_test
{
public:
  ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester() :
    ue_grid_allocator_default_cfg_test(([]() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.pdsch_nof_rbs        = {20, 40};
      sched_cfg_.ue.pusch_nof_rbs        = {20, 40};
      return sched_cfg_;
    }()))
  {
  }
};

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pdsch_with_expert_cfg_min_nof_rbs_even_if_rbs_required_to_schedule_is_low)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is low enough such that < 20 RBs (configured in constructor) are required to schedule.
  static const units::bytes sched_bytes{20U};
  const unsigned            max_nof_rbs_to_schedule = 10U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            std::max(expert_cfg.pdsch_nof_rbs.start(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pdsch_with_expert_cfg_max_nof_rbs_even_if_rbs_required_to_schedule_is_high)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 40 RBs (configured in constructor) are required to schedule.
  static const units::bytes sched_bytes{20000U};
  const unsigned            max_nof_rbs_to_schedule = 273U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            std::min(expert_cfg.pdsch_nof_rbs.stop(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pusch_with_expert_cfg_min_nof_rbs_even_if_rbs_required_to_schedule_is_low)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is low enough such that < 20 RBs (configured in constructor) are required to schedule.
  const units::bytes recommended_nof_bytes_to_schedule{20U};
  const unsigned     max_nof_rbs_to_schedule = 10U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(),
            std::max(expert_cfg.pdsch_nof_rbs.start(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pusch_with_expert_cfg_max_nof_rbs_even_if_rbs_required_to_schedule_is_high)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 40 RBs (configured in constructor) are required to schedule.
  const units::bytes recommended_nof_bytes_to_schedule{200000U};
  const unsigned     max_nof_rbs_to_schedule = 273U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(),
            std::min(expert_cfg.pdsch_nof_rbs.stop(), max_nof_rbs_to_schedule));
}

class ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester : public ue_grid_allocator_default_cfg_test
{
public:
  ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester() :
    ue_grid_allocator_default_cfg_test(([]() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.pdsch_crb_limits     = {20, 40};
      sched_cfg_.ue.pusch_crb_limits     = {20, 40};
      return sched_cfg_;
    }()))
  {
    // Assume SS#2 is USS configured with DCI format 1_1/0_1 and is the only SS used for UE PDSCH/PUSCH scheduling.
    const prb_interval pdsch_prbs =
        crb_to_prb(cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.crbs, sched_cfg.ue.pdsch_crb_limits);
    pdsch_vrb_limits = vrb_interval{pdsch_prbs.start(), pdsch_prbs.stop()};
    pusch_vrb_limits = rb_helper::crb_to_vrb_ul_non_interleaved(
        sched_cfg.ue.pusch_crb_limits, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
  }

protected:
  vrb_interval pdsch_vrb_limits;
  vrb_interval pusch_vrb_limits;
};

TEST_P(ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester, allocates_pdsch_within_expert_cfg_pdsch_rb_limits)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 20 RBs (configured in constructor) are required to schedule.
  static const units::bytes sched_bytes{20000U};
  const unsigned            max_nof_rbs_to_schedule = 273U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH within RB limits.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1(), pdsch_vrb_limits);
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester, allocates_pusch_within_expert_cfg_pusch_rb_limits)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 20 RBs (configured in constructor) are required to schedule.
  const units::bytes recommended_nof_bytes_to_schedule{200000U};
  const unsigned     max_nof_rbs_to_schedule = 273U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH within RB limits.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1(), pusch_vrb_limits);
}

class ue_grid_allocator_pdsch_repetition_test : public ue_grid_allocator_test
{
protected:
  static constexpr uint8_t nof_reps = 4;
  const lcid_t             drb_lcid = uint_to_lcid(4);

  ue_grid_allocator_pdsch_repetition_test() :
    ue_grid_allocator_test(test_params{[]() {
      auto cfg                       = config_helpers::make_default_scheduler_expert_config();
      cfg.ue.pdsch_cqi_rep_threshold = 6.0F;
      // Disable OLLA so the effective CQI equals the reported wideband CQI, making the repetition trigger
      // deterministic.
      cfg.ue.olla_cqi_inc = 0;
      return cfg;
    }()})
  {
  }

  // Adds a UE configured with a Rel-16 PDSCH TDRA list that mirrors the common list and appends a repetition entry.
  const ue& add_repetition_ue()
  {
    sched_ue_creation_request_message req =
        sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
    req.ue_index = to_du_ue_index(0);
    req.crnti    = to_rnti(0x4601);
    req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(drb_lcid));
    (*req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg = cell_cfg.bwp_res[to_bwp_id(0)].dl().ded_pdcchs[0];

    auto&       pdsch_cfg   = (*req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg.value();
    const auto& common_list = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& common_alloc : common_list) {
      pdsch_cfg.pdsch_td_alloc_list.push_back(common_alloc);
    }
    rep_time_resource                               = pdsch_cfg.pdsch_td_alloc_list.size();
    pdsch_time_domain_resource_allocation rep_alloc = common_list.front();
    rep_alloc.rep_number                            = nof_reps;
    pdsch_cfg.pdsch_td_alloc_list.push_back(rep_alloc);

    return add_ue(req);
  }

  void set_reported_cqi(ue_cell& ue_cc, uint8_t cqi)
  {
    csi_report_data csi{};
    csi.first_tb_wideband_cqi = csi_report_data::wideband_cqi_type{cqi};
    csi.valid                 = true;
    ue_cc.handle_csi_report(csi);
  }

  // Time-domain resource of the DL DCI (format 1_1) scheduled for the UE in the current slot, if any.
  std::optional<unsigned> current_dl_dci_time_resource(rnti_t rnti) const
  {
    const pdcch_dl_information* pdcch = find_ue_dl_pdcch(rnti, res_grid[0].result.dl);
    if (pdcch == nullptr or pdcch->dci.type() != dci_dl_rnti_config_type::c_rnti_f1_1) {
      return std::nullopt;
    }
    return pdcch->dci.as_c_rnti_f1_1().time_resource;
  }

  uint8_t rep_time_resource = 0;
};

// A HARQ reTx reuses the transmission scheme of the original transmission (like the number of layers), so the number
// of PDSCH repetitions is taken from the HARQ grant parameters, not re-decided from the current link quality. Verify
// that once the CQI recovers above the threshold, a reTx of a grant started at low CQI is still scheduled as a
// repetition bundle, while a fresh newTx is a single transmission.
TEST_P(ue_grid_allocator_pdsch_repetition_test, retx_reuses_original_repetition_scheme_after_cqi_recovers)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  auto ue_dl_grant = [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants); };

  // Low CQI: the newTx must be scheduled as a repetition bundle.
  set_reported_cqi(ue_cc, 3);
  push_dl_bs(u.ue_index, drb_lcid, 100000);
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], units::bytes{1000}, false); },
                        [&]() {
                          const dl_msg_alloc* g = ue_dl_grant();
                          return g != nullptr and g->context.nof_retxs == 0;
                        }));
  ASSERT_EQ(current_dl_dci_time_resource(u.crnti).value(), rep_time_resource)
      << "newTx at low CQI was not scheduled as a repetition bundle";

  // NACK the bundle to force a reTx.
  std::optional<dl_harq_process_handle> h_dl = ue_cc.harqs.find_dl_harq_waiting_ack();
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_dl->dl_ack_info(mac_harq_ack_report_status::nack, std::nullopt));

  // CQI recovers above the threshold. A fresh newTx would now be a single transmission, but the reTx must keep the
  // repetition scheme of the original transmission.
  set_reported_cqi(ue_cc, 15);
  ASSERT_TRUE(run_until(
      [&]() {
        std::optional<dl_harq_process_handle> h_retx = ue_cc.harqs.find_pending_dl_retx();
        if (h_retx.has_value()) {
          allocate_dl_retx_grant(slice_ues[u.ue_index], *h_retx);
        }
      },
      [&]() {
        const dl_msg_alloc* g = ue_dl_grant();
        return g != nullptr and g->context.nof_retxs > 0;
      }));
  ASSERT_EQ(current_dl_dci_time_resource(u.crnti).value(), rep_time_resource)
      << "reTx did not keep its repetition scheme after CQI recovered (reTx repetitions were incorrectly re-decided "
         "from the current CQI)";
}

// If a repetition bundle's grant is aborted (e.g. RB allocation failure) after the HARQ was allocated but before
// save_grant_params ever ran, the UE's last known PDSCH slot must be released immediately rather than needlessly
// kept reserved for the rest of the nominal bundle window (see dl_harq_process_impl::last_occasion_slot /
// cell_harq_repository::dealloc_harq): nothing was ever committed to the grid, so the UE must be schedulable again
// right away.
TEST_P(ue_grid_allocator_pdsch_repetition_test,
       when_newtx_bundle_aborts_before_commit_then_ue_is_immediately_schedulable_again)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  // Low CQI: the newTx qualifies for a repetition bundle.
  set_reported_cqi(ue_cc, 3);

  // Allocate the HARQ (and PDCCH/UCI) for a bundle, then abort it by committing empty VRBs, before the repetition
  // occasions are ever written to the grid.
  auto result = alloc.allocate_dl_grant(
      ue_newtx_dl_grant_request{slice_ues[u.ue_index], current_slot, units::bytes{1000}, false});
  ASSERT_TRUE(result.has_value());
  result.value().set_pdsch_params({}, {}, false);

  ASSERT_FALSE(ue_cc.harqs.last_pdsch_slot().valid());

  // The very next slot -- still nominally within the aborted bundle's window -- is immediately schedulable.
  slot_indication();
  auto retry = alloc.allocate_dl_grant(
      ue_newtx_dl_grant_request{slice_ues[u.ue_index], current_slot, units::bytes{1000}, false});
  ASSERT_TRUE(retry.has_value());
  retry.value().set_pdsch_params({}, {}, false);
}

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_pdsch_repetition_test,
                         testing::Values(duplex_mode::FDD));

class ue_grid_allocator_pusch_repetition_test : public ue_grid_allocator_test
{
protected:
  static constexpr uint8_t nof_reps = 4;
  const lcid_t             drb_lcid = uint_to_lcid(4);

  /// \param pusch_crb_limits CRB window the UE PUSCH is confined to.
  /// \param force_rep         Request repetitions regardless of the estimated SINR.
  explicit ue_grid_allocator_pusch_repetition_test(crb_interval pusch_crb_limits, bool force_rep = false) :
    ue_grid_allocator_test(test_params{[pusch_crb_limits, force_rep]() {
      auto cfg                        = config_helpers::make_default_scheduler_expert_config();
      cfg.ue.pusch_sinr_rep_threshold = 10.0F;
      // Disable UL OLLA so the effective SNR equals the value set on the channel state manager.
      cfg.ue.olla_ul_snr_inc  = 0;
      cfg.ue.pusch_crb_limits = pusch_crb_limits;
      cfg.ue.pusch_force_rep  = force_rep;
      return cfg;
    }()})
  {
  }

  // Shifts the PUSCH grant away from CRB 0, so an occasion's repeated PRBs stay clear of the cell's PUCCH.
  ue_grid_allocator_pusch_repetition_test() : ue_grid_allocator_pusch_repetition_test(crb_interval{10, 40}) {}

  // Adds a UE configured with a Rel-16 PUSCH TDRA list that mirrors the common list and appends a repetition entry.
  const ue& add_repetition_ue(std::optional<meas_gap_config>  meas_gap = std::nullopt,
                              std::optional<cg_configuration> cg_cfg   = std::nullopt)
  {
    sched_ue_creation_request_message req =
        sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);
    req.ue_index         = to_du_ue_index(0);
    req.crnti            = to_rnti(0x4601);
    req.cfg.meas_gap_cfg = meas_gap;
    if (cg_cfg.has_value()) {
      (*req.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.cg_cfg = std::move(cg_cfg);
    }
    req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(drb_lcid));
    (*req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg = cell_cfg.bwp_res[to_bwp_id(0)].dl().ded_pdcchs[0];

    auto&       pusch_cfg   = (*req.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.pusch_cfg.value();
    const auto& common_list = cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& common_alloc : common_list) {
      pusch_cfg.pusch_td_alloc_list.push_back(common_alloc);
    }
    rep_time_resource                               = pusch_cfg.pusch_td_alloc_list.size();
    pusch_time_domain_resource_allocation rep_alloc = common_list.front();
    rep_alloc.nof_repetitions                       = nof_reps;
    rep_k2                                          = rep_alloc.k2;
    pusch_cfg.pusch_td_alloc_list.push_back(rep_alloc);

    return add_ue(req);
  }

  /// Builds a Type 1 Configured Grant configuration whose only occasion within its period is \c cg_slot.
  static cg_configuration make_cg_config_at(slot_point cg_slot)
  {
    constexpr auto   period = cg_configuration::periodicity_t::sl80;
    cg_configuration cg{};
    cg.mcs_table          = pusch_mcs_table::qam64;
    cg.nof_harq_processes = 8;
    cg.periodicity        = period;

    cg_configuration::rrc_configured_ul_grant grant{};
    grant.time_domain_offset       = cg_slot.count() % static_cast<unsigned>(period);
    grant.time_domain_allocation   = 0;
    grant.freq_domain_res          = ra_frequency_type1_configuration{};
    grant.antenna_port             = 0;
    grant.precoding_and_nof_layers = 0;
    grant.mcs                      = 10;
    cg.rrc_configured_ul_grant_cfg = grant;
    return cg;
  }

  void set_pusch_snr(ue_cell& ue_cc, float snr_db) { ue_cc.channel_state_manager().update_pusch_snr(snr_db); }

  // Time-domain resource of the UL DCI (format 0_1) scheduled for the UE in the current slot, if any.
  std::optional<unsigned> current_ul_dci_time_resource(rnti_t rnti) const
  {
    const pdcch_ul_information* pdcch = find_ue_ul_pdcch(rnti, res_grid[0].result.dl);
    if (pdcch == nullptr or pdcch->dci.type() != dci_ul_rnti_config_type::c_rnti_f0_1) {
      return std::nullopt;
    }
    return pdcch->dci.as_c_rnti_f0_1().time_resource;
  }

  void allocate_ul_retx_grant(const slice_ue& user, ul_harq_process_handle h_ul)
  {
    // The repetition row's k2 is fixed (unlike DL's k0, commonly 0), so the candidate PUSCH slot is rep_k2 ahead
    // of the PDCCH slot, not current_slot itself.
    const slot_point pusch_slot   = current_slot + rep_k2;
    const auto&      init_ul_bwp  = cell_cfg.params.ul_cfg_common.init_ul_bwp;
    auto             used_ul_vrbs = res_grid[pusch_slot]
                            .ul_res_grid
                            .used_prbs(init_ul_bwp.generic_params.scs,
                                       init_ul_bwp.generic_params.crbs,
                                       init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols)
                            .convert_to<vrb_bitmap>();
    auto result = alloc.allocate_ul_grant(ue_retx_ul_grant_request{
        user, pusch_slot, h_ul, used_ul_vrbs, ofdm_symbol_range{0, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP}});
    if (result.has_value()) {
      used_ul_vrbs.fill(result.value().vrbs.start(), result.value().vrbs.stop());
    }
  }

  uint8_t rep_time_resource = 0;
  /// PDCCH-to-PUSCH delay (k2) of the repetition TDRA row, i.e. the offset from the PDCCH slot to occasion 0.
  uint8_t rep_k2 = 0;
};

// A reTx reuses the original transmission's scheme (like the number of layers), so its repetition count comes from
// the HARQ grant params, not the current link quality. Once the SINR recovers, a reTx of a grant started at low SINR
// must still be a bundle, while a fresh newTx is a single transmission.
TEST_P(ue_grid_allocator_pusch_repetition_test, retx_reuses_original_repetition_scheme_after_sinr_recovers)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  auto ue_ul_grant = [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul); };

  // The PUSCH repetition row's k2 is never 0, so the PDCCH (written to slot 0) and the base occasion's PUSCH
  // (written k2 ahead) are never both visible at res_grid[0] as run_until() advances one slot at a time. Capture the
  // DCI's time_resource when the PDCCH is created rather than re-deriving it later.
  std::optional<unsigned> newtx_time_resource;

  // Low SINR: the newTx must be scheduled as a repetition bundle.
  set_pusch_snr(ue_cc, 0.0F);
  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u.ue_index], units::bytes{1000});
        if (auto tr = current_ul_dci_time_resource(u.crnti); tr.has_value()) {
          newtx_time_resource = tr;
        }
      },
      [&]() {
        const ul_sched_info* g = ue_ul_grant();
        return g != nullptr and g->context.nof_retxs == 0;
      }));
  ASSERT_TRUE(newtx_time_resource.has_value());
  ASSERT_EQ(newtx_time_resource.value(), rep_time_resource)
      << "newTx at low SINR was not scheduled as a repetition bundle";

  // NACK the bundle to force a reTx.
  std::optional<ul_harq_process_handle> h_ul = ue_cc.harqs.find_ul_harq_waiting_ack();
  ASSERT_TRUE(h_ul.has_value());
  ASSERT_TRUE(h_ul->ul_crc_info(false).has_value());

  // SINR recovers: a fresh newTx would now be a single transmission, but the reTx must keep the bundle.
  std::optional<unsigned> retx_time_resource;
  set_pusch_snr(ue_cc, 30.0F);
  ASSERT_TRUE(run_until(
      [&]() {
        std::optional<ul_harq_process_handle> h_retx = ue_cc.harqs.find_pending_ul_retx();
        if (h_retx.has_value()) {
          allocate_ul_retx_grant(slice_ues[u.ue_index], *h_retx);
          if (auto tr = current_ul_dci_time_resource(u.crnti); tr.has_value()) {
            retx_time_resource = tr;
          }
        }
      },
      [&]() {
        const ul_sched_info* g = ue_ul_grant();
        return g != nullptr and g->context.nof_retxs > 0;
      }));
  ASSERT_TRUE(retx_time_resource.has_value());
  ASSERT_EQ(retx_time_resource.value(), rep_time_resource)
      << "reTx did not keep its repetition scheme after SINR recovered (reTx repetitions were incorrectly re-decided "
         "from the current SINR)";
}

// As per TS 38.213 Section 9.2.5.2 (confirmed against a real UE), a PUCCH overlapping a PUSCH with repetitions is
// not transmitted: the UE multiplexes its UCI onto the single overlapping occasion, and the remaining occasions
// carry UL-SCH only. So a dedicated HARQ-ACK PUCCH already scheduled in an occasion's slot -- for a DL PDSCH
// allocated independently of this bundle, say -- must end up on that occasion, with the PUCCH grant dropped.
TEST_P(ue_grid_allocator_pusch_repetition_test, uci_colliding_with_repetition_occasion_is_multiplexed_onto_it)
{
  const ue&                    u           = add_repetition_ue();
  ue_cell&                     ue_cc       = ues[u.ue_index].get_pcell();
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();

  // Reach a known, ready slot first: with no run_until below, every occasion's slot is then known in advance.
  slot_indication();

  // Occasion 3 (last of 4) lands at current_slot + rep_k2 + 3 -- FDD, so the offsets are consecutive.
  // Pre-allocate a dedicated HARQ-ACK PUCCH there.
  constexpr unsigned            target_occasion_offset = 3;
  const std::optional<unsigned> pucch_res_ind =
      pucch_alloc.alloc_ded_harq_ack(res_grid, ue_cell_cfg, 0, rep_k2 + target_occasion_offset);
  ASSERT_TRUE(pucch_res_ind.has_value());
  const slot_point occasion_slot = current_slot + rep_k2 + target_occasion_offset;
  ASSERT_FALSE(res_grid[occasion_slot].result.ul.pucchs.empty());

  set_pusch_snr(ue_cc, 0.0F);
  const slot_point pusch_slot = current_slot + rep_k2;
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  // The PUCCH must be gone: the UE does not transmit it, the UCI rides on the occasion instead.
  ASSERT_TRUE(res_grid[occasion_slot].result.ul.pucchs.empty())
      << "The PUCCH was left in place even though its UCI moved to the repetition occasion";

  // The colliding occasion must have been written, and must carry the HARQ-ACK bits taken off the PUCCH.
  const ul_sched_info* occ = find_ue_pusch(u.crnti, res_grid[occasion_slot].result.ul);
  ASSERT_NE(occ, nullptr) << "The repetition occasion colliding with the PUCCH was dropped instead of carrying the UCI";
  ASSERT_TRUE(occ->uci.has_value()) << "The occasion overlapping the PUCCH carries no UCI";
  ASSERT_TRUE(occ->uci->harq.has_value());
  ASSERT_GT(occ->uci->harq->harq_ack_nof_bits, 0);

  // The base occasion and every other occasion must be present and carry UL-SCH only.
  const ul_sched_info* base_occ = find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul);
  ASSERT_NE(base_occ, nullptr);
  ASSERT_FALSE(base_occ->uci.has_value());
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    if (offset == target_occasion_offset) {
      continue;
    }
    const ul_sched_info* other_occ = find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul);
    ASSERT_NE(other_occ, nullptr);
    ASSERT_FALSE(other_occ->uci.has_value()) << "UCI was multiplexed onto more than one repetition occasion";
  }
}

// The UL DAI covers the whole transmission, so a HARQ-ACK booked in a *repetition occasion's* slot must reach the
// DCI like one in the base slot. Counting only the base slot leaves the DAI at its "no HARQ-ACK" default (3, i.e.
// V_T_DAI_UL=4) while the gNB demaps a bit off the occasion: the codebooks disagree and the UCI is lost, silently.
TEST_P(ue_grid_allocator_pusch_repetition_test, harq_ack_booked_on_a_repetition_occasion_is_counted_in_the_ul_dai)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  slot_indication();

  // Book a HARQ-ACK through the UCI allocator (it maintains the per-slot counter the DAI derives from), forcing it
  // onto the slot of occasion 3.
  constexpr unsigned           target_occasion_offset = 3;
  const std::array<uint8_t, 1> k1_list                = {static_cast<uint8_t>(rep_k2 + target_occasion_offset)};
  ASSERT_TRUE(uci_alloc.alloc_harq_ack(res_grid, ue_cc, 0, k1_list, pucch_repetition_factor::n1).has_value());
  const slot_point occasion_slot = current_slot + rep_k2 + target_occasion_offset;
  ASSERT_EQ(uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(occasion_slot, u.crnti), 1);

  set_pusch_snr(ue_cc, 0.0F);
  const slot_point pusch_slot = current_slot + rep_k2;
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  // Find the UL grant's PDCCH and check the DAI it carries.
  const pdcch_ul_information* ul_pdcch = nullptr;
  for (const pdcch_ul_information& pdcch : res_grid[0].result.dl.ul_pdcchs) {
    if (pdcch.ctx.rnti == u.crnti) {
      ul_pdcch = &pdcch;
      break;
    }
  }
  ASSERT_NE(ul_pdcch, nullptr);
  ASSERT_EQ(ul_pdcch->dci.type(), dci_ul_rnti_config_type::c_rnti_f0_1);
  // One HARQ-ACK bit on the bundle: TS 38.213 Table 9.1.3-2 leftmost column, (1 - 1) % 4 == 0.
  ASSERT_EQ(ul_pdcch->dci.as_c_rnti_f0_1().first_dl_assignment_index, 0)
      << "The HARQ-ACK booked on a repetition occasion was not counted in the UL DAI";
}

// An occasion repeats the base grant's RBs -- that is what lets the PHY combine them -- so RBs busy in any occasion
// slot are unusable by the whole bundle. If the base grant ignores them the occasion is dropped, and the UE transmits
// there regardless: a collision with whoever got those RBs. The bundle must route around them.
TEST_P(ue_grid_allocator_pusch_repetition_test, bundle_avoids_rbs_busy_in_an_occasion_slot)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  slot_indication();

  // Occupy the lower half of the UE's PUSCH CRB window ({10, 40}) in the slot of occasion 3, standing in for another
  // UE's grant landing there first.
  constexpr unsigned target_occasion_offset = 3;
  const slot_point   occasion_slot          = current_slot + rep_k2 + target_occasion_offset;
  const crb_interval blocked_crbs{10, 25};
  res_grid[occasion_slot].ul_res_grid.fill(
      grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, ofdm_symbol_range{0, 14}, blocked_crbs});

  set_pusch_snr(ue_cc, 0.0F);
  const slot_point pusch_slot = current_slot + rep_k2;
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  const ul_sched_info* base_occ = find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul);
  ASSERT_NE(base_occ, nullptr);
  // The RBs picked must not touch the busy ones. Checking the base grant is what matters: the occasions repeat its
  // RBs verbatim (asserted below).
  const vrb_interval blocked_vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(
      blocked_crbs, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
  ASSERT_FALSE(base_occ->pusch_cfg.rbs.type1().overlaps(blocked_vrbs))
      << fmt::format("PUSCH VRBs {} overlap the VRBs {} already busy in the slot of occasion {}",
                     base_occ->pusch_cfg.rbs.type1(),
                     blocked_vrbs,
                     target_occasion_offset);

  // Every occasion must be present, and must reuse the base grant's RBs.
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    const ul_sched_info* occ = find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul);
    ASSERT_NE(occ, nullptr) << "Occasion at offset " << offset
                            << " was dropped: the bundle was sized against the base slot only";
    ASSERT_EQ(occ->pusch_cfg.rbs.type1(), base_occ->pusch_cfg.rbs.type1())
        << "Occasion at offset " << offset << " does not repeat the base grant's RBs";
  }
}

// The base slot is refused if the UE cannot transmit in it, and an occasion's slot has to be held to the same rule.
// A UE inside an uplink measurement gap leaves the uplink to measure (TS 38.133, Section 9.1C.2), while still
// counting the slot as available for repetition -- that count follows the semi-static UL/DL configuration alone --
// so the occasion is simply lost and the ones after it do not move up. The bundle must give way instead.
TEST_P(ue_grid_allocator_pusch_repetition_test, bundle_whose_occasion_falls_in_an_ul_meas_gap_gives_way_to_a_single_tx)
{
  // Reach a known, ready slot first: every occasion's slot is then known in advance.
  slot_indication();

  // Place a 6ms measurement gap on the slot of occasion 3. With no T_TA tracked the uplink window is neither shifted
  // nor guarded, so it covers the gap offset and the 6 subframes after it -- at the 15kHz of this suite, one slot per
  // subframe, which leaves the base slot and the earlier occasions outside.
  constexpr unsigned target_occasion_offset = 3;
  const uint8_t      common_k2 = cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2;
  const slot_point   gap_slot  = current_slot + common_k2 + target_occasion_offset;
  const unsigned     gap_offset =
      (gap_slot.count() / gap_slot.nof_slots_per_subframe()) % static_cast<unsigned>(meas_gap_repetition_period::ms80);

  const ue& u = add_repetition_ue(meas_gap_config{gap_offset, meas_gap_length::ms6, meas_gap_repetition_period::ms80});
  ue_cell&  ue_cc             = ues[u.ue_index].get_pcell();
  const slot_point pusch_slot = current_slot + rep_k2;
  ASSERT_EQ(pusch_slot + target_occasion_offset, gap_slot);
  ASSERT_TRUE(ue_cc.is_ul_enabled(pusch_slot)) << "the base slot itself fell in the gap, the test proves nothing";
  ASSERT_FALSE(ue_cc.is_ul_enabled(gap_slot));

  set_pusch_snr(ue_cc, 0.0F);
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  const std::optional<unsigned> time_resource = current_ul_dci_time_resource(u.crnti);
  ASSERT_TRUE(time_resource.has_value());
  ASSERT_NE(time_resource.value(), rep_time_resource)
      << "a bundle was scheduled over a slot the UE spends in a measurement gap";

  // A single transmission occupies its own slot alone, so no occasion reaches the grid.
  ASSERT_NE(find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul), nullptr);
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    ASSERT_EQ(find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul), nullptr)
        << "Occasion at offset " << offset << " was scheduled for a grant downgraded to a single transmission";
  }
}

// Likewise for a slot the UE holds for a Configured Grant: the base slot never takes one (the UL scheduling context
// gives up on it), and an occasion must not either, or the dynamic bundle and the UE's own CG PUSCH would land in the
// same slot.
TEST_P(ue_grid_allocator_pusch_repetition_test, bundle_whose_occasion_falls_on_a_cg_slot_gives_way_to_a_single_tx)
{
  // Reach a known, ready slot first: every occasion's slot is then known in advance.
  slot_indication();

  constexpr unsigned target_occasion_offset = 3;
  const uint8_t      common_k2 = cell_cfg.params.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].k2;
  const slot_point   cg_slot   = current_slot + common_k2 + target_occasion_offset;

  const ue&        u          = add_repetition_ue(std::nullopt, make_cg_config_at(cg_slot));
  ue_cell&         ue_cc      = ues[u.ue_index].get_pcell();
  const slot_point pusch_slot = current_slot + rep_k2;
  ASSERT_EQ(pusch_slot + target_occasion_offset, cg_slot);
  ASSERT_FALSE(ue_cc.cfg().is_cg_slot(pusch_slot)) << "the base slot itself is a CG slot, the test proves nothing";
  ASSERT_TRUE(ue_cc.cfg().is_cg_slot(cg_slot));

  set_pusch_snr(ue_cc, 0.0F);
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  const std::optional<unsigned> time_resource = current_ul_dci_time_resource(u.crnti);
  ASSERT_TRUE(time_resource.has_value());
  ASSERT_NE(time_resource.value(), rep_time_resource)
      << "a bundle was scheduled over a slot the UE holds for a Configured Grant";

  // A single transmission occupies its own slot alone, so no occasion reaches the grid.
  ASSERT_NE(find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul), nullptr);
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    ASSERT_EQ(find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul), nullptr)
        << "Occasion at offset " << offset << " was scheduled for a grant downgraded to a single transmission";
  }
}

// The reTx counterpart of the test above, and a stricter one: a reTx must repeat the original transmission's RB
// count exactly, so it cannot shrink around the busy RBs the way a newTx can -- it has to move aside as a whole.
TEST_P(ue_grid_allocator_pusch_repetition_test, retx_bundle_avoids_rbs_busy_in_an_occasion_slot)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  slot_indication();

  // Low SINR: the newTx is scheduled as a bundle. Cap its RBs, so that the reTx, which needs the very same number of
  // them, still has room to land elsewhere in the UE's PUSCH CRB window ({10, 40}).
  constexpr unsigned nof_grant_rbs = 10;
  set_pusch_snr(ue_cc, 0.0F);
  ASSERT_EQ(allocate_ul_newtx_grant(current_slot + rep_k2, slice_ues[u.ue_index], units::bytes{1000}, nof_grant_rbs),
            alloc_status::success);
  const ul_sched_info* newtx_grant = find_ue_pusch(u.crnti, res_grid[current_slot + rep_k2].result.ul);
  ASSERT_NE(newtx_grant, nullptr);
  ASSERT_EQ(newtx_grant->pusch_cfg.rbs.type1().length(), nof_grant_rbs);

  // NACK the bundle and step past it, so the reTx is searched on a grid clear of the original transmission.
  std::optional<ul_harq_process_handle> h_ul = ue_cc.harqs.find_ul_harq_waiting_ack();
  ASSERT_TRUE(h_ul.has_value());
  ASSERT_TRUE(h_ul->ul_crc_info(false).has_value());
  for (unsigned i = 0; i != nof_reps; ++i) {
    slot_indication();
  }
  std::optional<ul_harq_process_handle> h_retx = ue_cc.harqs.find_pending_ul_retx();
  ASSERT_TRUE(h_retx.has_value());

  // Occupy, in the slot of occasion 3, the RBs at the bottom of the UE's PUSCH CRB window -- exactly where the reTx
  // would land, its own slot being free.
  constexpr unsigned target_occasion_offset = 3;
  const slot_point   pusch_slot             = current_slot + rep_k2;
  const crb_interval blocked_crbs{10, 10 + nof_grant_rbs};
  res_grid[pusch_slot + target_occasion_offset].ul_res_grid.fill(
      grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs, ofdm_symbol_range{0, 14}, blocked_crbs});
  const vrb_interval blocked_vrbs = rb_helper::crb_to_vrb_ul_non_interleaved(
      blocked_crbs, cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
  ASSERT_EQ(newtx_grant->pusch_cfg.rbs.type1(), blocked_vrbs)
      << "the blocked RBs are not the ones the reTx would pick, so the test would pass without moving anything";

  allocate_ul_retx_grant(slice_ues[u.ue_index], *h_retx);

  const ul_sched_info* retx_grant = find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul);
  ASSERT_NE(retx_grant, nullptr) << "the reTx bundle was not scheduled at all";
  ASSERT_EQ(retx_grant->context.nof_retxs, 1);
  ASSERT_EQ(retx_grant->pusch_cfg.rbs.type1().length(), nof_grant_rbs);
  ASSERT_FALSE(retx_grant->pusch_cfg.rbs.type1().overlaps(blocked_vrbs))
      << fmt::format("reTx VRBs {} overlap the VRBs {} already busy in the slot of occasion {}",
                     retx_grant->pusch_cfg.rbs.type1(),
                     blocked_vrbs,
                     target_occasion_offset);

  // Every occasion must be present, and must reuse the reTx grant's RBs.
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    const ul_sched_info* occ = find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul);
    ASSERT_NE(occ, nullptr) << "Occasion at offset " << offset
                            << " was dropped: the reTx bundle was sized against the base slot only";
    ASSERT_EQ(occ->pusch_cfg.rbs.type1(), retx_grant->pusch_cfg.rbs.type1())
        << "Occasion at offset " << offset << " does not repeat the reTx grant's RBs";
  }
}

// The UE multiplexes in each slot of a bundle the HARQ-ACKs booked for that slot, and the single UL DAI of the
// bundle's only DCI applies to every one of those slots alike (TS 38.213, clause 9). Slots whose codebooks need
// different DAI values therefore cannot be served by one bundle: the grant gives way to a single transmission, which
// answers for its own slot only. Scheduling the bundle anyway would size the UE's codebook wrongly in at least one
// slot, and the UCI there would be lost silently.
TEST_P(ue_grid_allocator_pusch_repetition_test, bundle_whose_slots_need_different_ul_dai_gives_way_to_a_single_tx)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  // Reach a known, ready slot first: with no run_until below, every occasion's slot is then known in advance.
  slot_indication();

  const slot_point pusch_slot = current_slot + rep_k2;
  // One HARQ-ACK on the base occasion's slot and two on occasion 3's: their codebooks need DAI values
  // (1 - 1) % 4 == 0 and (2 - 1) % 4 == 1, which one DCI cannot carry at once.
  constexpr unsigned           target_occasion_offset = 3;
  const std::array<uint8_t, 1> base_k1                = {rep_k2};
  const std::array<uint8_t, 1> occasion_k1            = {static_cast<uint8_t>(rep_k2 + target_occasion_offset)};
  ASSERT_TRUE(uci_alloc.alloc_harq_ack(res_grid, ue_cc, 0, base_k1, pucch_repetition_factor::n1).has_value());
  for (unsigned i = 0; i != 2; ++i) {
    ASSERT_TRUE(uci_alloc.alloc_harq_ack(res_grid, ue_cc, 0, occasion_k1, pucch_repetition_factor::n1).has_value());
  }
  const slot_point occasion_slot = pusch_slot + target_occasion_offset;
  ASSERT_EQ(uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(pusch_slot, u.crnti), 1);
  ASSERT_EQ(uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(occasion_slot, u.crnti), 2);

  set_pusch_snr(ue_cc, 0.0F);
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  const std::optional<unsigned> time_resource = current_ul_dci_time_resource(u.crnti);
  ASSERT_TRUE(time_resource.has_value());
  ASSERT_NE(time_resource.value(), rep_time_resource)
      << "A bundle was scheduled over slots that cannot share one UL DAI";

  // A single transmission occupies its own slot alone, so no occasion reaches the grid.
  ASSERT_NE(find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul), nullptr);
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    ASSERT_EQ(find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul), nullptr)
        << "Occasion at offset " << offset << " was scheduled for a grant downgraded to a single transmission";
  }
}

// The counterpart of the test above: slots agreeing on the UL DAI keep their bundle. Together they pin down that it
// is the DAI that decides, not the mere presence of HARQ-ACK in several slots of the bundle.
TEST_P(ue_grid_allocator_pusch_repetition_test, bundle_whose_slots_agree_on_the_ul_dai_is_scheduled)
{
  const ue& u     = add_repetition_ue();
  ue_cell&  ue_cc = ues[u.ue_index].get_pcell();

  slot_indication();

  const slot_point pusch_slot = current_slot + rep_k2;
  // Two HARQ-ACKs in each of the two slots: both codebooks need a DAI of (2 - 1) % 4 == 1.
  constexpr unsigned           target_occasion_offset = 3;
  const std::array<uint8_t, 1> base_k1                = {rep_k2};
  const std::array<uint8_t, 1> occasion_k1            = {static_cast<uint8_t>(rep_k2 + target_occasion_offset)};
  for (unsigned i = 0; i != 2; ++i) {
    ASSERT_TRUE(uci_alloc.alloc_harq_ack(res_grid, ue_cc, 0, base_k1, pucch_repetition_factor::n1).has_value());
    ASSERT_TRUE(uci_alloc.alloc_harq_ack(res_grid, ue_cc, 0, occasion_k1, pucch_repetition_factor::n1).has_value());
  }
  const slot_point occasion_slot = pusch_slot + target_occasion_offset;
  ASSERT_EQ(uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(pusch_slot, u.crnti), 2);
  ASSERT_EQ(uci_alloc.get_scheduled_pdsch_counter_in_ue_uci(occasion_slot, u.crnti), 2);

  set_pusch_snr(ue_cc, 0.0F);
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  const std::optional<unsigned> time_resource = current_ul_dci_time_resource(u.crnti);
  ASSERT_TRUE(time_resource.has_value());
  ASSERT_EQ(time_resource.value(), rep_time_resource)
      << "The bundle was given up even though its slots agree on the UL DAI";
  for (unsigned offset = 1; offset != nof_reps; ++offset) {
    ASSERT_NE(find_ue_pusch(u.crnti, res_grid[pusch_slot + offset].result.ul), nullptr)
        << "Occasion at offset " << offset << " is missing from the bundle";
  }

  // The DAI describes one slot's codebook, which both slots share -- not their sum.
  const pdcch_ul_information* ul_pdcch = nullptr;
  for (const pdcch_ul_information& pdcch : res_grid[0].result.dl.ul_pdcchs) {
    if (pdcch.ctx.rnti == u.crnti) {
      ul_pdcch = &pdcch;
      break;
    }
  }
  ASSERT_NE(ul_pdcch, nullptr);
  ASSERT_EQ(ul_pdcch->dci.as_c_rnti_f0_1().first_dl_assignment_index, 1)
      << "The UL DAI does not describe the 2 HARQ-ACK bits each slot of the bundle reports";
}

// Same setup, but with the UE PUSCH free to span the whole BWP, as in a real deployment. Every repetition occasion
// then overlaps in RBs with the UE's own PUCCH resources, which sit at the BWP edges.
class ue_grid_allocator_pusch_repetition_wideband_test : public ue_grid_allocator_pusch_repetition_test
{
protected:
  ue_grid_allocator_pusch_repetition_wideband_test() :
    ue_grid_allocator_pusch_repetition_test(crb_interval{0, MAX_NOF_PRBS})
  {
  }
};

// Regression: with the PUSCH free to span the BWP, a PUCCH in an occasion slot used to cost that occasion -- the
// grant was sized against the base slot alone, so its PRBs ran into the PUCCH and the occasion was dropped. The
// grant now routes around everything busy in every slot of the bundle.
TEST_P(ue_grid_allocator_pusch_repetition_wideband_test, pucch_in_an_occasion_slot_does_not_break_the_bundle)
{
  const ue&                    u           = add_repetition_ue();
  ue_cell&                     ue_cc       = ues[u.ue_index].get_pcell();
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();

  slot_indication();

  constexpr unsigned            target_occasion_offset = 3;
  const std::optional<unsigned> pucch_res_ind =
      pucch_alloc.alloc_ded_harq_ack(res_grid, ue_cell_cfg, 0, rep_k2 + target_occasion_offset);
  ASSERT_TRUE(pucch_res_ind.has_value());
  const slot_point occasion_slot = current_slot + rep_k2 + target_occasion_offset;
  ASSERT_FALSE(res_grid[occasion_slot].result.ul.pucchs.empty());

  set_pusch_snr(ue_cc, 0.0F);
  const slot_point pusch_slot = current_slot + rep_k2;
  ASSERT_EQ(allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], units::bytes{1000}), alloc_status::success);

  // The base grant must be wide enough to overlap the PUCCH, or this degenerates into the narrowband test.
  const ul_sched_info* base_occ = find_ue_pusch(u.crnti, res_grid[pusch_slot].result.ul);
  ASSERT_NE(base_occ, nullptr);
  ASSERT_GT(base_occ->pusch_cfg.rbs.type1().length(), 40U) << "PUSCH is too narrow to exercise the RB overlap";

  ASSERT_TRUE(res_grid[occasion_slot].result.ul.pucchs.empty());
  const ul_sched_info* occ = find_ue_pusch(u.crnti, res_grid[occasion_slot].result.ul);
  ASSERT_NE(occ, nullptr) << "The occasion overlapping the UE's own PUCCH in RBs was dropped";
  ASSERT_TRUE(occ->uci.has_value());
  ASSERT_TRUE(occ->uci->harq.has_value());
  ASSERT_GT(occ->uci->harq->harq_ack_nof_bits, 0);
}

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_pusch_repetition_wideband_test,
                         testing::Values(duplex_mode::FDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_pusch_repetition_test,
                         testing::Values(duplex_mode::FDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_css_test,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_default_cfg_test,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));
