// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/ran_resource_management/du_pdsch_resource_manager.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/rrm/cell_rrm_config.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

class du_pdsch_res_alloc_resource_manager_test : public ::testing::TestWithParam<resource_allocation>
{
protected:
  static du_cell_config make_cell_config(const cell_config_builder_params& params)
  {
    du_cell_config cell               = config_helpers::make_default_du_cell_config(params);
    cell.ran.init_bwp.pdsch.res_alloc = GetParam();
    cell.ran.init_bwp.pdsch.rbg_sz    = rbg_size::config2;
    return cell;
  }

  cell_group_config make_ue() const
  {
    cell_group_config cfg;
    cfg.cells.emplace(SERVING_PCELL_IDX, config_helpers::make_default_ue_cell_config(cell_cfg_list.front().ran));
    return cfg;
  }

  static ue_capability_summary make_caps(bool dyn_switch_supported)
  {
    ue_capability_summary caps;
    caps.pdsch_dyn_switch_ra_type0_1_supported = dyn_switch_supported;
    return caps;
  }

  static const ue_pdsch_rrm_config& get_pdsch_rrm(const cell_group_config& ue)
  {
    return ue.cells.at(SERVING_PCELL_IDX).rrm.pdsch;
  }

  cell_config_builder_params  params{cell_config_builder_profiles::create(duplex_mode::TDD)};
  std::vector<du_cell_config> cell_cfg_list{make_cell_config(params)};
  du_test_mode_config         test_cfg{};
  du_pdsch_resource_manager   pdsch_mng{cell_cfg_list, test_cfg};
};

TEST_P(du_pdsch_res_alloc_resource_manager_test, when_ue_capabilities_are_unknown_then_type1_is_used)
{
  cell_group_config ue = make_ue();
  pdsch_mng.alloc_resources(ue);

  ASSERT_EQ(get_pdsch_rrm(ue).res_alloc, resource_allocation::resource_allocation_type_1);
}

TEST_P(du_pdsch_res_alloc_resource_manager_test, when_ue_supports_dynamic_switch_then_configured_type_is_used)
{
  cell_group_config ue = make_ue();
  pdsch_mng.alloc_resources(ue);
  pdsch_mng.update_resources(ue, make_caps(true));

  ASSERT_EQ(get_pdsch_rrm(ue).res_alloc, GetParam());
  ASSERT_EQ(make_cell_rrm_config(cell_cfg_list.front().ran).pdsch.rbg_sz, rbg_size::config2);
}

TEST_P(du_pdsch_res_alloc_resource_manager_test, when_ue_does_not_support_dynamic_switch_then_it_is_not_used)
{
  cell_group_config ue = make_ue();
  pdsch_mng.alloc_resources(ue);
  pdsch_mng.update_resources(ue, make_caps(false));

  // Resource allocation type 0 is mandatory for UEs, while the dynamic switch is a UE capability.
  const resource_allocation expected =
      GetParam() == resource_allocation::dynamic_switch ? resource_allocation::resource_allocation_type_1 : GetParam();
  ASSERT_EQ(get_pdsch_rrm(ue).res_alloc, expected);
}

INSTANTIATE_TEST_SUITE_P(du_pdsch_res_alloc_resource_manager_test,
                         du_pdsch_res_alloc_resource_manager_test,
                         ::testing::Values(resource_allocation::resource_allocation_type_0,
                                           resource_allocation::resource_allocation_type_1,
                                           resource_allocation::dynamic_switch));

/// Tests the PDSCH values of a UE that depend on the capabilities of the UE.
class du_pdsch_resource_manager_test : public ::testing::Test
{
protected:
  du_pdsch_resource_manager_test()
  {
    cell_cfg_list.front().ran.init_bwp.pdsch.mcs_table                = pdsch_mcs_table::qam256;
    cell_cfg_list.front().ran.init_bwp.pdsch.interleaving_bundle_size = vrb_to_prb::mapping_type::interleaved_n4;
  }

  /// Makes the PDSCH parameters of a UE, which the manager configures with the given capabilities.
  ue_pdsch_rrm_config make_ue_pdsch(const std::optional<ue_capability_summary>& caps)
  {
    cell_group_config ue;
    ue.cells.emplace(SERVING_PCELL_IDX, config_helpers::make_default_ue_cell_config(cell_cfg_list.front().ran));
    if (caps.has_value()) {
      pdsch_mng.update_resources(ue, *caps);
    } else {
      pdsch_mng.alloc_resources(ue);
    }
    return ue.cells.at(SERVING_PCELL_IDX).rrm.pdsch;
  }

  cell_config_builder_params  params{cell_config_builder_profiles::create(duplex_mode::TDD)};
  std::vector<du_cell_config> cell_cfg_list{config_helpers::make_default_du_cell_config(params)};
  du_test_mode_config         test_cfg{};
  du_pdsch_resource_manager   pdsch_mng{cell_cfg_list, test_cfg};
};

TEST_F(du_pdsch_resource_manager_test, ue_uses_the_mcs_table_of_the_cell_only_if_it_supports_it)
{
  ue_capability_summary caps;

  // Without the UE capabilities, the UE uses the lowest MCS table.
  ASSERT_EQ(make_ue_pdsch(std::nullopt).mcs_table, pdsch_mcs_table::qam64);

  caps.pdsch_qam256_supported = false;
  ASSERT_EQ(make_ue_pdsch(caps).mcs_table, pdsch_mcs_table::qam64);

  caps.pdsch_qam256_supported = true;
  ASSERT_EQ(make_ue_pdsch(caps).mcs_table, pdsch_mcs_table::qam256);
}

TEST_F(du_pdsch_resource_manager_test, ue_uses_the_vrb_to_prb_interleaving_of_the_cell_only_if_it_supports_it)
{
  ue_capability_summary caps;

  // Without the UE capabilities, the UE uses no interleaving.
  ASSERT_EQ(make_ue_pdsch(std::nullopt).interleaving, vrb_to_prb::mapping_type::non_interleaved);

  caps.pdsch_interleaving_vrb_to_prb_supported = false;
  ASSERT_EQ(make_ue_pdsch(caps).interleaving, vrb_to_prb::mapping_type::non_interleaved);

  caps.pdsch_interleaving_vrb_to_prb_supported = true;
  ASSERT_EQ(make_ue_pdsch(caps).interleaving, vrb_to_prb::mapping_type::interleaved_n4);
}
