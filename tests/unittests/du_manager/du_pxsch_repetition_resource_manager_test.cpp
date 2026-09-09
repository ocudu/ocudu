// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/ran_resource_management/du_pdsch_resource_manager.h"
#include "lib/du/du_high/du_manager/ran_resource_management/du_pusch_resource_manager.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

static du_cell_config
make_test_cell_config(const cell_config_builder_params& params, unsigned max_dl_reps, unsigned max_ul_reps)
{
  du_cell_config cell                         = config_helpers::make_default_du_cell_config(params);
  cell.ran.init_bwp.pdsch.max_nof_repetitions = max_dl_reps;
  cell.ran.init_bwp.pusch.max_nof_repetitions = max_ul_reps;
  return cell;
}

class du_pxsch_repetition_resource_manager_test : public ::testing::Test
{
protected:
  cell_group_config make_ue() const
  {
    cell_group_config cfg;
    cfg.cells.emplace(SERVING_PCELL_IDX, config_helpers::make_default_ue_cell_config(cell_cfg_list.front().ran));
    return cfg;
  }

  ue_capability_summary make_caps(uint8_t max_pdsch_tdra_rep_number, bool pusch_rep_supported) const
  {
    ue_capability_summary caps;
    caps.pusch_rep_type_a_supported = pusch_rep_supported;
    ue_capability_summary::supported_band band_caps;
    band_caps.max_pdsch_tdra_rep_number             = max_pdsch_tdra_rep_number;
    band_caps.pusch_rep_type_a_avail_slot_supported = pusch_rep_supported;
    caps.bands.emplace(cell_cfg_list.front().ran.dl_carrier.band, band_caps);
    return caps;
  }

  static const pdsch_config& get_pdsch_cfg(const cell_group_config& ue)
  {
    return ue.cells.at(SERVING_PCELL_IDX).serv_cell_cfg.init_dl_bwp.pdsch_cfg.value();
  }

  static const pusch_config& get_pusch_cfg(const cell_group_config& ue)
  {
    return ue.cells.at(SERVING_PCELL_IDX).serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value();
  }

  cell_config_builder_params  params{cell_config_builder_profiles::create(duplex_mode::TDD)};
  std::vector<du_cell_config> cell_cfg_list{make_test_cell_config(params, 8, 8)};
  du_test_mode_config         test_cfg{};
  du_pdsch_resource_manager   pdsch_mng{cell_cfg_list, test_cfg};
  du_pusch_resource_manager   pusch_mng{cell_cfg_list, test_cfg};
};

TEST_F(du_pxsch_repetition_resource_manager_test, when_ue_supports_pdsch_repetitions_then_td_alloc_list_r16_is_built)
{
  cell_group_config ue = make_ue();
  pdsch_mng.update_resources(ue, make_caps(16, false));

  const auto& common_list = cell_cfg_list.front().ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  const auto& list        = get_pdsch_cfg(ue).pdsch_td_alloc_list;

  // The list mirrors the common list entries and appends a single entry with repetitionNumber=min(cell=8, UE=16)=8.
  ASSERT_EQ(list.size(), common_list.size() + 1);
  for (unsigned i = 0; i != common_list.size(); ++i) {
    ASSERT_EQ(list[i], common_list[i]);
    ASSERT_FALSE(list[i].rep_number.has_value());
  }
  const auto& rep_entry = list.back();
  ASSERT_EQ(rep_entry.rep_number, 8);
  // The repetition entry reuses the full-slot common entry.
  ASSERT_EQ(rep_entry.k0, common_list[0].k0);
  ASSERT_EQ(rep_entry.map_type, common_list[0].map_type);
  ASSERT_EQ(rep_entry.symbols, common_list[0].symbols);

  // The slot-based repetition scheme must be enabled along with the Rel-16 TDRA list.
  ASSERT_TRUE(get_pdsch_cfg(ue).slot_based_repetition_enabled);
}

TEST_F(du_pxsch_repetition_resource_manager_test, when_ue_caps_limit_pdsch_repetitions_then_range_is_reduced)
{
  cell_group_config ue = make_ue();
  pdsch_mng.update_resources(ue, make_caps(2, false));

  const auto& common_list = cell_cfg_list.front().ran.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  const auto& list        = get_pdsch_cfg(ue).pdsch_td_alloc_list;

  // min(cell=8, UE=2) = 2 -> single repetition entry with repetitionNumber=2.
  ASSERT_EQ(list.size(), common_list.size() + 1);
  ASSERT_EQ(list.back().rep_number, 2);
}

TEST_F(du_pxsch_repetition_resource_manager_test, when_ue_does_not_support_pdsch_repetitions_then_list_r16_is_empty)
{
  cell_group_config ue = make_ue();

  // No UE capabilities available.
  pdsch_mng.alloc_resources(ue);
  ASSERT_TRUE(get_pdsch_cfg(ue).pdsch_td_alloc_list.empty());
  ASSERT_FALSE(get_pdsch_cfg(ue).slot_based_repetition_enabled);

  // UE capabilities without supportRepNumPDSCH-TDRA-r16.
  pdsch_mng.update_resources(ue, make_caps(1, false));
  ASSERT_TRUE(get_pdsch_cfg(ue).pdsch_td_alloc_list.empty());
  ASSERT_FALSE(get_pdsch_cfg(ue).slot_based_repetition_enabled);
}

TEST_F(du_pxsch_repetition_resource_manager_test, when_cell_disables_pdsch_repetitions_then_list_r16_is_empty)
{
  std::vector<du_cell_config> cells{make_test_cell_config(params, 1, 1)};
  du_pdsch_resource_manager   mng{cells, test_cfg};

  cell_group_config ue = make_ue();
  mng.update_resources(ue, make_caps(16, true));
  ASSERT_TRUE(get_pdsch_cfg(ue).pdsch_td_alloc_list.empty());
}

TEST_F(du_pxsch_repetition_resource_manager_test, when_ue_supports_pusch_repetitions_then_dci_0_1_list_r16_is_built)
{
  cell_group_config ue = make_ue();
  pusch_mng.update_resources(ue, make_caps(1, true));

  const auto& common_list =
      cell_cfg_list.front().ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
  const auto& list = get_pusch_cfg(ue).pusch_td_alloc_list;

  // The list mirrors the common list entries and appends a single entry with numberOfRepetitions=8.
  ASSERT_EQ(list.size(), common_list.size() + 1);
  for (unsigned i = 0; i != common_list.size(); ++i) {
    ASSERT_EQ(list[i], common_list[i]);
    ASSERT_FALSE(list[i].nof_repetitions.has_value());
  }
  const auto& rep_entry = list.back();
  ASSERT_EQ(rep_entry.nof_repetitions, 8);
  // The repetition entry reuses the full-slot common entry with the lowest k2.
  ASSERT_EQ(rep_entry.k2, common_list[0].k2);
  ASSERT_EQ(rep_entry.map_type, common_list[0].map_type);
  ASSERT_EQ(rep_entry.symbols, common_list[0].symbols);
}

// The dedicated list must reach exactly the same slots as the common one: the UL slice scheduler derives a cell's
// candidate PUSCH slots from the common list alone, before any UE is considered, so it has no dedicated list to
// consult. A k2 offered only by the dedicated list would name a slot that never gets a slice candidate, making grants
// that use it silently impossible; a k2 dropped from it would have candidate slots generated that a UE on DCI format
// 0_1 could never use.
TEST_F(du_pxsch_repetition_resource_manager_test, dedicated_pusch_list_offers_exactly_the_k2_values_of_the_common_one)
{
  cell_group_config ue = make_ue();
  pusch_mng.update_resources(ue, make_caps(1, true));

  const auto& common_list =
      cell_cfg_list.front().ran.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
  const auto& ded_list = get_pusch_cfg(ue).pusch_td_alloc_list;
  ASSERT_FALSE(ded_list.empty());

  const auto offers_k2 = [](const auto& list, uint8_t k2) {
    return std::any_of(
        list.begin(), list.end(), [k2](const pusch_time_domain_resource_allocation& res) { return res.k2 == k2; });
  };
  for (const pusch_time_domain_resource_allocation& ded : ded_list) {
    EXPECT_TRUE(offers_k2(common_list, ded.k2)) << "dedicated k2=" << unsigned{ded.k2}
                                                << " is not offered by the "
                                                   "common list";
  }
  for (const pusch_time_domain_resource_allocation& common : common_list) {
    EXPECT_TRUE(offers_k2(ded_list, common.k2)) << "common k2=" << unsigned{common.k2}
                                                << " is missing from the "
                                                   "dedicated list";
  }
}

TEST_F(du_pxsch_repetition_resource_manager_test, when_ue_does_not_support_pusch_repetitions_then_list_r16_is_empty)
{
  cell_group_config ue = make_ue();

  // No UE capabilities available.
  pusch_mng.alloc_resources(ue);
  ASSERT_TRUE(get_pusch_cfg(ue).pusch_td_alloc_list.empty());

  // UE capabilities without pusch-RepetitionTypeA-r16/puschTypeA-RepetitionsAvailSlot-r17 support.
  pusch_mng.update_resources(ue, make_caps(16, false));
  ASSERT_TRUE(get_pusch_cfg(ue).pusch_td_alloc_list.empty());
}
