// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/converters/asn1_cell_group_release_helpers.h"
#include "lib/du/du_high/du_manager/converters/asn1_rrc_config_helpers.h"
#include "ocudu/asn1/rrc_nr/cell_group_config.h"
#include "ocudu/mac/config/mac_cell_group_config_factory.h"
#include "ocudu/mac/config/mac_config_helpers.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace asn1::rrc_nr;

/// IDs that only the source cell group configures.
static constexpr uint8_t SOURCE_ONLY_NZP_CSI_RS_RES_ID = 100;
static constexpr uint8_t SOURCE_ONLY_LCID              = 5;
static constexpr uint8_t SOURCE_RELEASED_PUCCH_RES_ID  = 30;

class cell_group_config_release_test : public ::testing::Test
{
protected:
  cell_group_config_release_test()
  {
    odu::du_ue_resource_config res_cfg{};
    res_cfg.cell_group.cells.emplace(
        SERVING_PCELL_IDX, config_helpers::make_default_ue_cell_config(config_helpers::make_default_ran_cell_config()));
    res_cfg.cell_group.mcg_cfg = config_helpers::make_initial_mac_cell_group_config();
    res_cfg.srbs.emplace(srb_id_t::srb1);
    auto& srb1   = res_cfg.srbs[srb_id_t::srb1];
    srb1.srb_id  = srb_id_t::srb1;
    srb1.rlc_cfg = make_default_srb_rlc_config();
    srb1.mac_cfg = make_default_srb_mac_lc_config(LCID_SRB1);

    // The source and the target start with the same complete configuration.
    odu::calculate_cell_group_config_diff(target, {}, res_cfg);
    source = target;
  }

  static pucch_cfg_s& pucch_cfg(cell_group_cfg_s& cell_group)
  {
    return cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup();
  }

  static byte_buffer pack(const cell_group_cfg_s& cell_group)
  {
    byte_buffer   buf;
    asn1::bit_ref bref{buf};
    EXPECT_EQ(asn1::pack(bref, cell_group), asn1::OCUDUASN_SUCCESS);
    return buf;
  }

  cell_group_cfg_s target;
  cell_group_cfg_s source;
};

TEST_F(cell_group_config_release_test, when_source_and_target_are_equal_then_no_release_is_added)
{
  const byte_buffer target_before = pack(target);

  odu::calculate_cell_group_config_release(target, source);

  ASSERT_EQ(pack(target), target_before);
}

TEST_F(cell_group_config_release_test, when_source_has_more_pucch_resources_then_the_extra_resources_are_released)
{
  // The source cell is wider than the target cell and configures more PUCCH resources.
  auto&         src_pucch   = pucch_cfg(source);
  const uint8_t nof_tgt_res = src_pucch.res_to_add_mod_list.size();
  for (uint8_t id = nof_tgt_res; id != nof_tgt_res + 3; ++id) {
    pucch_res_s res  = src_pucch.res_to_add_mod_list[0];
    res.pucch_res_id = id;
    src_pucch.res_to_add_mod_list.push_back(res);
  }

  odu::calculate_cell_group_config_release(target, source);

  const auto& tgt_pucch = pucch_cfg(target);
  ASSERT_EQ(tgt_pucch.res_to_release_list.size(), 3);
  for (uint8_t i = 0; i != 3; ++i) {
    ASSERT_EQ(tgt_pucch.res_to_release_list[i], nof_tgt_res + i);
  }
  ASSERT_EQ(tgt_pucch.res_to_add_mod_list.size(), nof_tgt_res);
  ASSERT_EQ(tgt_pucch.res_set_to_release_list.size(), 0);
  ASSERT_EQ(tgt_pucch.sched_request_res_to_release_list.size(), 0);
}

TEST_F(cell_group_config_release_test, when_target_has_more_pucch_resources_then_no_release_is_added)
{
  // The target cell is wider than the source cell and configures more PUCCH resources.
  auto&       tgt_pucch = pucch_cfg(target);
  pucch_res_s res       = tgt_pucch.res_to_add_mod_list[0];
  res.pucch_res_id      = tgt_pucch.res_to_add_mod_list.size();
  tgt_pucch.res_to_add_mod_list.push_back(res);
  const byte_buffer target_before = pack(target);

  odu::calculate_cell_group_config_release(target, source);

  ASSERT_EQ(pack(target), target_before);
}

TEST_F(cell_group_config_release_test, when_target_does_not_set_up_a_field_of_the_source_then_the_field_is_released)
{
  auto& tgt_ded = target.sp_cell_cfg.sp_cell_cfg_ded;
  ASSERT_TRUE(source.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg_present);
  ASSERT_TRUE(source.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.is_setup());
  tgt_ded.csi_meas_cfg_present = false;
  ASSERT_TRUE(pucch_cfg(source).format2_present);
  pucch_cfg(target).format2_present = false;

  odu::calculate_cell_group_config_release(target, source);

  ASSERT_TRUE(tgt_ded.csi_meas_cfg_present);
  ASSERT_EQ(tgt_ded.csi_meas_cfg.type().value, asn1::setup_release_opts::release);
  ASSERT_TRUE(pucch_cfg(target).format2_present);
  ASSERT_EQ(pucch_cfg(target).format2.type().value, asn1::setup_release_opts::release);
}

TEST_F(cell_group_config_release_test, when_source_has_more_csi_resources_then_the_extra_resources_are_released)
{
  auto& src_csi = source.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup();
  ASSERT_GT(src_csi.nzp_csi_rs_res_to_add_mod_list.size(), 0);
  nzp_csi_rs_res_s res  = src_csi.nzp_csi_rs_res_to_add_mod_list[0];
  res.nzp_csi_rs_res_id = SOURCE_ONLY_NZP_CSI_RS_RES_ID;
  src_csi.nzp_csi_rs_res_to_add_mod_list.push_back(res);

  odu::calculate_cell_group_config_release(target, source);

  const auto& tgt_csi = target.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup();
  ASSERT_EQ(tgt_csi.nzp_csi_rs_res_to_release_list.size(), 1);
  ASSERT_EQ(tgt_csi.nzp_csi_rs_res_to_release_list[0], SOURCE_ONLY_NZP_CSI_RS_RES_ID);
}

TEST_F(cell_group_config_release_test, when_source_has_more_rlc_bearers_then_the_extra_bearers_are_released)
{
  rlc_bearer_cfg_s bearer = source.rlc_bearer_to_add_mod_list[0];
  bearer.lc_ch_id         = SOURCE_ONLY_LCID;
  source.rlc_bearer_to_add_mod_list.push_back(bearer);

  odu::calculate_cell_group_config_release(target, source);

  ASSERT_EQ(target.rlc_bearer_to_release_list.size(), 1);
  ASSERT_EQ(target.rlc_bearer_to_release_list[0], SOURCE_ONLY_LCID);
}

TEST_F(cell_group_config_release_test, when_source_releases_an_element_then_no_release_is_added)
{
  // The source contains a release of a PUCCH resource. The UE does not hold this resource.
  pucch_cfg(source).res_to_release_list.push_back(SOURCE_RELEASED_PUCCH_RES_ID);
  const byte_buffer target_before = pack(target);

  odu::calculate_cell_group_config_release(target, source);

  ASSERT_EQ(pack(target), target_before);
}
