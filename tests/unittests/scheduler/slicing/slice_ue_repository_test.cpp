// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/slicing/slice_ue_repository.h"
#include "lib/scheduler/ue_context/ue_cell_repository.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

struct test_lc_ch_cfg {
  lcid_t                      lcid;
  lcg_id_t                    lcg_id;
  ran_slice_id_t              slice_id;
  std::optional<ul_harq_mode> allowed_harq_mode = std::nullopt;
};

ul_bsr_indication_message create_short_bsr(du_ue_index_t ue_idx, ul_bsr_lcg_report_list report)
{
  return ul_bsr_indication_message{
      to_du_cell_index(0), ue_idx, to_rnti(0x4601 + ue_idx), bsr_format::SHORT_BSR, report};
}

} // namespace

class slice_ue_repository_test : public ::testing::Test
{
protected:
  slice_ue_repository_test() :
    test_cfg(
        []() {
          cell_config_builder_params params{};
          params.scs_common             = subcarrier_spacing::kHz30;
          params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz100;
          params.dl_carrier.arfcn_f_ref = 520002;
          params.dl_carrier.band        = nr_band::n41;
          return params;
        }(),
        expert_cfg),
    ue_db(expert_cfg.ue)
  {
    ocudulog::init();

    auto req = test_cfg.get_default_cell_config_request();
    test_cfg.add_cell(req);

    next_slot = {to_numerology_value(req.ran.dl_cfg_common.init_dl_bwp.generic_params.scs), 0};

    cell_ues.emplace(test_cfg.get_cell(to_du_cell_index(0)), nullptr);
    ue_db.register_cell(*cell_ues);

    slices.push_back(std::make_unique<slice_ue_repository>(SRB_RAN_SLICE_ID, du_cell_index_t(0), ue_db));
    slices.push_back(std::make_unique<slice_ue_repository>(DEFAULT_DRB_RAN_SLICE_ID, du_cell_index_t(0), ue_db));
    slices.push_back(std::make_unique<slice_ue_repository>(ran_slice_id_t{2}, du_cell_index_t(0), ue_db));
  }

  void add_ue(du_ue_index_t              ue_idx,
              span<const test_lc_ch_cfg> lc_chs,
              bool                       starts_in_fallback = false,
              std::optional<slot_point>  ul_ccch_slot_rx    = std::nullopt)
  {
    auto req               = test_cfg.get_default_ue_config_request();
    req.ue_index           = ue_idx;
    req.crnti              = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback = starts_in_fallback;
    req.ul_ccch_slot_rx    = ul_ccch_slot_rx;
    std::vector<logical_channel_config> lc_cfg_list;
    lc_cfg_list.reserve(lc_chs.size() + 1);
    lc_cfg_list.push_back(logical_channel_config{LCID_SRB0, uint_to_lcg_id(0)});
    for (const auto& lc_ch : lc_chs) {
      lc_cfg_list.push_back(logical_channel_config{lc_ch.lcid, lc_ch.lcg_id});
      lc_cfg_list.back().allowed_harq_mode = lc_ch.allowed_harq_mode;
    }
    req.cfg.lc_config_list         = lc_cfg_list;
    const ue_configuration* ue_cfg = test_cfg.add_ue(req);
    ue_db.add_ue(*ue_cfg, {sched_config_helper::to_ue_creation_mode(req), req.ul_ccch_slot_rx});

    for (const auto& lc_ch : lc_chs) {
      slices[lc_ch.slice_id.value()]->add_logical_channel(ue_db[ue_idx], lc_ch.lcid, lc_ch.lcg_id);
    }
  }

  // add_ue that automatically adds UE to all slices.
  void add_ue(du_ue_index_t             ue_idx,
              bool                      starts_in_fallback = false,
              std::optional<slot_point> ul_ccch_slot_rx    = std::nullopt)
  {
    std::vector<test_lc_ch_cfg> lc_chs = {
        test_lc_ch_cfg{LCID_SRB1, uint_to_lcg_id(0), SRB_RAN_SLICE_ID},
        test_lc_ch_cfg{LCID_MIN_DRB, uint_to_lcg_id(1), DEFAULT_DRB_RAN_SLICE_ID},
        test_lc_ch_cfg{uint_to_lcid(LCID_MIN_DRB + 1), uint_to_lcg_id(2), ran_slice_id_t{2}}};
    add_ue(ue_idx, lc_chs, starts_in_fallback, ul_ccch_slot_rx);
  }

  /// Marks the given number of bytes as allocated in a UL HARQ of the UE, so that they count as already in flight.
  void allocate_ul_harq_bytes(du_ue_index_t ue_idx, units::bytes tbs, std::optional<ran_slice_id_t> slice_id)
  {
    auto h_ul = ue_db[ue_idx].get_pcell().harqs.alloc_ul_harq(next_slot, next_slot, 4);
    report_fatal_error_if_not(h_ul.has_value(), "No UL HARQ process available");
    ul_harq_alloc_context ctxt;
    ctxt.dci_cfg_type = dci_ul_rnti_config_type::c_rnti_f0_1;
    ctxt.slice_id     = slice_id;
    pusch_information pusch;
    pusch.rnti          = to_rnti(0x4601 + ue_idx);
    pusch.harq_id       = h_ul->id();
    pusch.new_data      = true;
    pusch.tb_size_bytes = tbs;
    h_ul->save_grant_params(ctxt, pusch);
  }

  ocudulog::basic_logger&                 logger = ocudulog::fetch_basic_logger("TEST");
  const scheduler_expert_config           expert_cfg;
  test_helpers::test_sched_config_manager test_cfg;
  std::optional<ue_cell_repository>       cell_ues;
  // Second cell of the UE repository, for the tests that need one.
  std::optional<ue_cell_repository>                 cell1_ues;
  ue_repository                                     ue_db;
  std::vector<std::unique_ptr<slice_ue_repository>> slices;

  slot_point next_slot;
};

TEST_F(slice_ue_repository_test, add_ue_to_slice_repo_alters_repo_size)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);

  ASSERT_TRUE(slices[SRB_RAN_SLICE_ID.value()]->empty());
  ASSERT_EQ(slices[SRB_RAN_SLICE_ID.value()]->size(), 0);
  ASSERT_FALSE(slices[SRB_RAN_SLICE_ID.value()]->contains(ue_idx));
  add_ue(ue_idx);
  ASSERT_FALSE(slices[SRB_RAN_SLICE_ID.value()]->empty());
  ASSERT_EQ(slices[SRB_RAN_SLICE_ID.value()]->size(), 1);
  ASSERT_TRUE(slices[SRB_RAN_SLICE_ID.value()]->contains(ue_idx));
}

TEST_F(slice_ue_repository_test, sr_and_ce_of_ue_in_fallback_are_not_pending_newtx_data_of_srb_slice)
{
  const du_ue_index_t active_idx   = to_du_ue_index(0);
  const du_ue_index_t fallback_idx = to_du_ue_index(3);
  add_ue(active_idx);
  // UE created by the upper layers, in fallback until its C-RNTI CE is received.
  add_ue(fallback_idx, true);
  const slice_ue_repository& srb_slice = *slices[SRB_RAN_SLICE_ID.value()];

  for (du_ue_index_t ue_idx : {active_idx, fallback_idx}) {
    ue_db[ue_idx].handle_sr_indication(next_slot);
    ue_db[ue_idx].handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_idx, lcid_dl_sch_t::TA_CMD});
  }

  // The SR stays visible to the UE and to the fallback scheduler, but not to the SRB slice.
  ASSERT_TRUE(ue_db[fallback_idx].logical_channels().has_pending_sr());
  ASSERT_TRUE(ue_db.get_fallback_ues_with_pending_newtx_data(to_du_cell_index(0), false).test(fallback_idx));
  for (bool is_dl : {true, false}) {
    const bounded_bitset<MAX_NOF_DU_UES> pending_ues = srb_slice.get_ues_with_pending_newtx_data(is_dl);
    ASSERT_TRUE(pending_ues.test(active_idx));
    ASSERT_FALSE(pending_ues.size() > fallback_idx and pending_ues.test(fallback_idx));
  }

  ASSERT_TRUE(ue_db.crnti_ce_received(fallback_idx));
  ASSERT_FALSE(ue_db.get_fallback_ues_with_pending_newtx_data(to_du_cell_index(0), false).any());
  for (bool is_dl : {true, false}) {
    ASSERT_TRUE(srb_slice.get_ues_with_pending_newtx_data(is_dl).test(fallback_idx));
  }
}

TEST_F(slice_ue_repository_test, sr_and_ce_of_deactivated_ue_are_not_pending_newtx_data_of_srb_slice)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  // UE created by a RACH, whose ConRes CE is never acked.
  add_ue(ue_idx, true, next_slot);
  const slice_ue_repository& srb_slice = *slices[SRB_RAN_SLICE_ID.value()];

  ASSERT_TRUE(ue_db.handle_conres_ce_outcome(ue_idx, false));
  ue_db[ue_idx].handle_sr_indication(next_slot);
  ue_db[ue_idx].handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_idx, lcid_dl_sch_t::TA_CMD});

  for (bool is_dl : {true, false}) {
    ASSERT_TRUE(srb_slice.get_ues_with_pending_newtx_data(is_dl).none());
    ASSERT_TRUE(ue_db.get_fallback_ues_with_pending_newtx_data(to_du_cell_index(0), is_dl).none());
  }
}

TEST_F(slice_ue_repository_test, ues_with_pending_newtx_data_only_include_ues_of_the_slice_cell)
{
  // Add a second cell to the same UE repository.
  auto cell_req       = test_cfg.get_default_cell_config_request();
  cell_req.cell_index = to_du_cell_index(1);
  test_cfg.add_cell(cell_req);
  cell1_ues.emplace(test_cfg.get_cell(to_du_cell_index(1)), nullptr);
  ue_db.register_cell(*cell1_ues);
  slice_ue_repository cell1_srb_slice(SRB_RAN_SLICE_ID, to_du_cell_index(1), ue_db);

  // UE 0 in cell 0 and UE 1 in cell 1, both with SRB1 data.
  const du_ue_index_t cell0_ue_idx = to_du_ue_index(0);
  const du_ue_index_t cell1_ue_idx = to_du_ue_index(1);
  add_ue(cell0_ue_idx);
  auto ue_req                                          = test_cfg.get_default_ue_config_request();
  ue_req.ue_index                                      = cell1_ue_idx;
  ue_req.crnti                                         = to_rnti(0x4601 + cell1_ue_idx);
  ue_req.starts_in_fallback                            = false;
  (*ue_req.cfg.cells)[0].cell_index                    = to_du_cell_index(1);
  (*ue_req.cfg.cells)[0].ded->serv_cell_cfg.cell_index = to_du_cell_index(1);
  const ue_configuration* ue_cfg                       = test_cfg.add_ue(ue_req);
  ue_db.add_ue(*ue_cfg, {sched_config_helper::to_ue_creation_mode(ue_req), ue_req.ul_ccch_slot_rx});
  cell1_srb_slice.add_logical_channel(ue_db[cell1_ue_idx], LCID_SRB1, uint_to_lcg_id(0));
  for (du_ue_index_t ue_idx : {cell0_ue_idx, cell1_ue_idx}) {
    ue_db[ue_idx].handle_dl_buffer_state_indication(LCID_SRB1, 100);
  }

  const bounded_bitset<MAX_NOF_DU_UES> cell0_ues =
      slices[SRB_RAN_SLICE_ID.value()]->get_ues_with_pending_newtx_data(true);
  ASSERT_TRUE(cell0_ues.test(cell0_ue_idx));
  ASSERT_EQ(cell0_ues.count(), 1);
  const bounded_bitset<MAX_NOF_DU_UES> cell1_ues_with_data = cell1_srb_slice.get_ues_with_pending_newtx_data(true);
  ASSERT_TRUE(cell1_ues_with_data.test(cell1_ue_idx));
  ASSERT_EQ(cell1_ues_with_data.count(), 1);
}

TEST_F(slice_ue_repository_test, if_positive_dl_bs_added_to_slice_then_slice_ue_has_pending_bytes)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  auto& ue_srb_slice = (*slices[SRB_RAN_SLICE_ID.value()])[ue_idx];

  ASSERT_EQ(ue_srb_slice.pending_dl_newtx_bytes(), 0);
  const unsigned bs_val = 1000;
  ue_db[ue_idx].handle_dl_buffer_state_indication(LCID_SRB1, bs_val);
  ASSERT_GE(ue_srb_slice.pending_dl_newtx_bytes(), bs_val);
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_dl_newtx_bytes(), 0);
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_dl_newtx_bytes(), 0);
}

TEST_F(slice_ue_repository_test, if_positive_bsr_added_to_slice_then_slice_ue_has_pending_bytes)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  auto& ue_srb_slice = (*slices[SRB_RAN_SLICE_ID.value()])[ue_idx];

  ASSERT_EQ(ue_srb_slice.pending_ul_newtx_bytes(), units::bytes{0});
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{0}, bsr_val}}));
  ASSERT_GE(ue_srb_slice.pending_ul_newtx_bytes(), units::bytes{1000});
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
}

TEST_F(slice_ue_repository_test, if_sr_detected_and_no_pending_bsr_data_then_pending_data_is_positive_for_srb_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);

  ue_db[ue_idx].handle_sr_indication(next_slot);
  ASSERT_GT((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
}

TEST_F(slice_ue_repository_test,
       if_sr_detected_and_there_is_pending_bsr_data_in_other_slices_then_pending_data_is_not_positive_for_srb_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));

  ue_db[ue_idx].handle_sr_indication(next_slot);
  ASSERT_EQ((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
  ASSERT_GE((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{bsr_val});
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
}

TEST_F(
    slice_ue_repository_test,
    if_sr_detected_and_there_is_no_pending_bsr_data_in_other_slices_due_to_allocated_harqs_then_pending_data_is_positive_for_srb_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));
  allocate_ul_harq_bytes(ue_idx, units::bytes{2000}, DEFAULT_DRB_RAN_SLICE_ID);

  ue_db[ue_idx].handle_sr_indication(next_slot);
  ASSERT_GT((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), units::bytes{0});
}

/// A restricted logical channel with pending data decides the UL HARQ mode of the grants of its slice.
TEST_F(slice_ue_repository_test, restricted_logical_channel_decides_the_ul_harq_mode_of_its_slice)
{
  const du_ue_index_t               ue_idx = to_du_ue_index(0);
  const std::vector<test_lc_ch_cfg> lc_chs = {
      test_lc_ch_cfg{LCID_SRB1, uint_to_lcg_id(0), SRB_RAN_SLICE_ID},
      test_lc_ch_cfg{LCID_MIN_DRB, uint_to_lcg_id(1), DEFAULT_DRB_RAN_SLICE_ID, ul_harq_mode::mode_a}};
  add_ue(ue_idx, lc_chs);
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, 1000}}));

  const slice_ue& u         = (*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx];
  const auto      selection = u.select_ul_grant();
  ASSERT_TRUE(selection.restrictions.harq_mode.has_value());
  ASSERT_EQ(*selection.restrictions.harq_mode, ul_harq_mode::mode_a);
}

/// With no channel restricting it, a grant rides mode B, whichever RAN slice it belongs to.
TEST_F(slice_ue_repository_test, unrestricted_data_rides_mode_b_in_every_slice)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, 1000}}));

  ASSERT_FALSE((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].select_ul_grant().restrictions.is_restricted());
  ASSERT_FALSE((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].select_ul_grant().restrictions.is_restricted());
}

/// The grant an SR earns follows the channels it is meant to serve, which hold no data to restrict it yet.
TEST_F(slice_ue_repository_test, sr_grant_follows_the_configured_restriction)
{
  const du_ue_index_t               ue_idx = to_du_ue_index(0);
  const std::vector<test_lc_ch_cfg> lc_chs = {
      test_lc_ch_cfg{LCID_SRB1, uint_to_lcg_id(0), SRB_RAN_SLICE_ID, ul_harq_mode::mode_a},
      test_lc_ch_cfg{LCID_MIN_DRB, uint_to_lcg_id(1), DEFAULT_DRB_RAN_SLICE_ID, ul_harq_mode::mode_b}};
  add_ue(ue_idx, lc_chs);
  ue_db[ue_idx].handle_sr_indication(next_slot);

  const slice_ue& u = (*slices[SRB_RAN_SLICE_ID.value()])[ue_idx];
  ASSERT_EQ(u.select_ul_grant().restrictions.harq_mode, ul_harq_mode::mode_a);
  // The SR is reported per UE, so the other slices see it too and keep following their own channels.
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].select_ul_grant().restrictions.harq_mode,
            ul_harq_mode::mode_b);
}

/// \brief A slice is not held back by the data another slice of the same UE has in flight.
///
/// The bytes in flight come off the pending ones so that a buffer status the UE has not refreshed yet is not granted
/// twice. A transmission of another slice fills a grant this one is not sized for, so it leaves it untouched.
TEST_F(slice_ue_repository_test, data_in_flight_of_another_slice_does_not_hold_back_a_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));
  const slice_ue& u = (*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx];
  ASSERT_GE(u.pending_ul_newtx_bytes(), units::bytes{bsr_val});

  // Another slice of the UE holds more in flight than this one reports pending.
  allocate_ul_harq_bytes(ue_idx, units::bytes{10000}, ran_slice_id_t{2});

  EXPECT_GE(u.pending_ul_newtx_bytes(), units::bytes{bsr_val}) << "held back by the data another slice has in flight";
}

/// The bytes a slice already has in flight are not granted to it a second time.
TEST_F(slice_ue_repository_test, data_in_flight_of_a_slice_comes_off_its_own_pending_bytes)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 4000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));
  const slice_ue&    u      = (*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx];
  const units::bytes before = u.pending_ul_newtx_bytes();
  ASSERT_GE(before, units::bytes{bsr_val});

  const units::bytes in_flight{1000};
  allocate_ul_harq_bytes(ue_idx, in_flight, DEFAULT_DRB_RAN_SLICE_ID);

  EXPECT_EQ(u.pending_ul_newtx_bytes(), before - in_flight);
}

/// \brief A transmission no slice claims is charged to every one of them.
///
/// Only the grants of the slice scheduler carry the slice that filled them. A UE leaving fallback still owes the
/// bytes of the grant that served it there, and the slice they belong to must not be given them a second time.
TEST_F(slice_ue_repository_test, data_in_flight_of_no_slice_counts_against_every_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 4000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));
  const slice_ue&    u              = (*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx];
  const units::bytes before         = u.pending_ul_newtx_bytes();
  const units::bytes ue_wide_before = ue_db[ue_idx].pending_ul_newtx_bytes();
  ASSERT_GE(before, units::bytes{bsr_val});

  const units::bytes in_flight{1000};
  allocate_ul_harq_bytes(ue_idx, in_flight, std::nullopt);

  EXPECT_EQ(u.pending_ul_newtx_bytes(), before - in_flight);
  // The fallback scheduler, which serves the UE outside of any slice, counts them just the same.
  EXPECT_EQ(ue_db[ue_idx].pending_ul_newtx_bytes(), ue_wide_before - in_flight);
}

/// \brief A group whose data is in flight must not take the grant from the groups the restriction serves.
///
/// The bytes in flight are counted for the whole slice, so they cannot be told apart from the ones the restriction
/// leaves out. Taking them off what the grant serves, rather than off the slice total, would empty it.
TEST_F(slice_ue_repository_test, data_in_flight_does_not_starve_the_groups_a_restricted_grant_serves)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  // One slice, two groups that disagree, so a grant serves one of them and leaves the other out.
  const std::vector<test_lc_ch_cfg> lc_chs = {
      test_lc_ch_cfg{LCID_SRB1, uint_to_lcg_id(0), SRB_RAN_SLICE_ID},
      test_lc_ch_cfg{LCID_MIN_DRB, uint_to_lcg_id(1), DEFAULT_DRB_RAN_SLICE_ID, ul_harq_mode::mode_a},
      test_lc_ch_cfg{
          uint_to_lcid(LCID_MIN_DRB + 1), uint_to_lcg_id(2), DEFAULT_DRB_RAN_SLICE_ID, ul_harq_mode::mode_b}};
  add_ue(ue_idx, lc_chs);
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, 1000}}));
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{2}, 8000}}));

  const slice_ue& u      = (*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx];
  const auto      before = u.select_ul_grant();
  ASSERT_EQ(before.restrictions.harq_mode, ul_harq_mode::mode_a) << "mode A outranks mode B with data";
  const unsigned served_bytes = before.pending_bytes;
  ASSERT_GT(served_bytes, 0);

  // More bytes are in flight than the group the grant serves holds, and they may well be the other group's.
  allocate_ul_harq_bytes(ue_idx, units::bytes{4000}, DEFAULT_DRB_RAN_SLICE_ID);

  const auto selection = u.select_ul_grant();
  ASSERT_EQ(selection.restrictions.harq_mode, ul_harq_mode::mode_a);
  EXPECT_EQ(selection.pending_bytes, served_bytes) << "the group the grant serves was starved by data in flight";
}

/// \brief The grant an SR earns is not cut down to the bytes the slice already has in flight.
///
/// A grant that serves every group of the slice is sized by the slice alone, the minimum an SR earns included.
TEST_F(slice_ue_repository_test, an_unrestricted_grant_keeps_the_minimum_an_sr_earns)
{
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  // The SRB slice holds less than the minimum, and all of it is already in flight.
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{0}, 100}}));
  allocate_ul_harq_bytes(ue_idx, units::bytes{2000}, SRB_RAN_SLICE_ID);
  ue_db[ue_idx].handle_sr_indication(next_slot);

  const slice_ue& u         = (*slices[SRB_RAN_SLICE_ID.value()])[ue_idx];
  const auto      selection = u.select_ul_grant();
  ASSERT_FALSE(selection.restrictions.is_restricted());
  EXPECT_EQ(units::bytes{selection.pending_bytes}, u.pending_ul_newtx_bytes());
  EXPECT_GT(selection.pending_bytes, ue_db[ue_idx].logical_channels().ul_pending_bytes(SRB_RAN_SLICE_ID))
      << "the grant was cut down to the bytes the slice holds";
}
