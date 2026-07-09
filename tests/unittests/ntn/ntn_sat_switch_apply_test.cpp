// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/ntn/ntn_sat_switch_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu_ntn;

namespace {

ntn_cell_config make_base_config()
{
  ntn_cell_config cfg{};
  cfg.si_msg_idx          = 0;
  cfg.si_period_rf        = 1;
  cfg.si_window_len_slots = 1;
  cfg.si_window_position  = 1;

  ntn_serving_cell_config serving{};
  serving.satellite_index          = 1;
  serving.cell_specific_koffset    = std::chrono::milliseconds{10};
  serving.ntn_ul_sync_validity_dur = 30U;
  serving.k_mac                    = 2U;
  serving.ta_report                = true;
  serving.use_state_vector         = true;
  serving.reference_location       = geodetic_coordinates_t{1.0, 2.0, 3.0};
  serving.t_service                = std::chrono::system_clock::time_point{std::chrono::seconds{2000}};
  cfg.ntn_cfg                      = serving;

  ntn_neighbor_cell_config ncell{};
  ncell.satellite_index = 5;
  cfg.ncells.push_back(ncell);

  return cfg;
}

ntn_sat_switch_config make_sat_switch(unsigned satellite_index)
{
  ntn_sat_switch_config sw{};
  sw.satellite_index = satellite_index;
  sw.t_service_start = std::chrono::system_clock::time_point{std::chrono::seconds{1000}};
  return sw;
}

} // namespace

TEST(derive_post_switch_config_test, returns_nullopt_when_no_sat_switch)
{
  ntn_cell_config cfg = make_base_config();
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value());
}

TEST(derive_post_switch_config_test, returns_nullopt_when_no_serving_ntn_cfg)
{
  ntn_cell_config cfg = make_base_config();
  cfg.ntn_cfg.reset();
  cfg.sat_switch = make_sat_switch(9);
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value());
}

TEST(derive_post_switch_config_test, returns_nullopt_when_serving_has_no_t_service)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9);
  cfg.ntn_cfg->t_service.reset();
  EXPECT_FALSE(derive_post_switch_config(cfg).has_value())
      << "per TS 38.331 clause 5.7.19 the switch only executes when t-Service is broadcast";
}

TEST(derive_post_switch_config_test, switches_satellite_index_and_clears_sat_switch)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9);

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  ASSERT_TRUE(derived->ntn_cfg.has_value());
  EXPECT_EQ(derived->ntn_cfg->satellite_index, 9U);
  EXPECT_FALSE(derived->sat_switch.has_value());
  EXPECT_FALSE(derived->ntn_cfg->t_service.has_value())
      << "t_service described the source satellite's service stop and must not survive the switch";
}

TEST(derive_post_switch_config_test, applies_sat_switch_overrides_when_set)
{
  ntn_cell_config cfg                      = make_base_config();
  cfg.sat_switch                           = make_sat_switch(9);
  cfg.sat_switch->ntn_ul_sync_validity_dur = 60U;
  cfg.sat_switch->cell_specific_koffset    = std::chrono::milliseconds{20};
  cfg.sat_switch->k_mac                    = 4U;
  cfg.sat_switch->ta_report                = false;
  cfg.sat_switch->use_state_vector         = false;
  cfg.sat_switch->polarization = ntn_polarization_t{ntn_polarization_t::polarization_type::lhcp, std::nullopt};

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  EXPECT_EQ(derived->ntn_cfg->ntn_ul_sync_validity_dur, 60U);
  EXPECT_EQ(derived->ntn_cfg->cell_specific_koffset, std::chrono::milliseconds{20});
  EXPECT_EQ(*derived->ntn_cfg->k_mac, 4U);
  EXPECT_EQ(*derived->ntn_cfg->ta_report, false);
  EXPECT_EQ(*derived->ntn_cfg->use_state_vector, false);
  ASSERT_TRUE(derived->ntn_cfg->polarization.has_value());
  EXPECT_EQ(*derived->ntn_cfg->polarization->dl, ntn_polarization_t::polarization_type::lhcp);
}

TEST(derive_post_switch_config_test, falls_back_to_current_value_when_sat_switch_leaves_field_unset)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9); // leaves ntn_ul_sync_validity_dur, k_mac, etc. unset

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  EXPECT_EQ(derived->ntn_cfg->ntn_ul_sync_validity_dur, 30U); // unchanged from make_base_config()
  EXPECT_EQ(derived->ntn_cfg->cell_specific_koffset, std::chrono::milliseconds{10});
  EXPECT_EQ(*derived->ntn_cfg->k_mac, 2U);
  EXPECT_EQ(*derived->ntn_cfg->ta_report, true);
  EXPECT_EQ(*derived->ntn_cfg->use_state_vector, true);
}

TEST(derive_post_switch_config_test, preserves_ncells_and_non_sat_switch_fields)
{
  ntn_cell_config cfg = make_base_config();
  cfg.sat_switch      = make_sat_switch(9);

  auto derived = derive_post_switch_config(cfg);
  ASSERT_TRUE(derived.has_value());
  ASSERT_EQ(derived->ncells.size(), 1U);
  EXPECT_EQ(derived->ncells[0].satellite_index, 5U);
  ASSERT_TRUE(derived->ntn_cfg->reference_location.has_value());
  EXPECT_DOUBLE_EQ(derived->ntn_cfg->reference_location->latitude, 1.0);
}
