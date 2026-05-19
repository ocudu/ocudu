// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/support/pucch/pucch_collision_info.h"
#include "lib/scheduler/support/pucch/pucch_default_resource.h"
#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include <gtest/gtest.h>
#include <optional>

using namespace ocudu;

static constexpr bwp_configuration test_bwp_cfg{
    .cp   = cyclic_prefix::NORMAL,
    .scs  = subcarrier_spacing::kHz30,
    .crbs = crb_interval::start_and_len(0, 25),
};

static void check_resources_do_not_collide_with_each_other(std::vector<pucch_collision_info>& infos)
{
  for (unsigned i = 0; i != infos.size(); ++i) {
    for (unsigned j = 0; j != infos.size(); ++j) {
      if (i == j) {
        ASSERT_TRUE(infos[i].collides(infos[j]));
      } else {
        ASSERT_FALSE(infos[i].collides(infos[j]));
      }
    }
  }
}

TEST(pucch_collision_info_test, common_resources_do_not_collide)
{
  static constexpr bwp_configuration bwp_cfg{
      .cp   = cyclic_prefix::NORMAL,
      .scs  = subcarrier_spacing::kHz30,
      .crbs = crb_interval::start_and_len(0, 25),
  };

  for (unsigned row_index = 0; row_index != 16; ++row_index) {
    auto default_res = get_pucch_default_resource(11, bwp_cfg.crbs.length());

    std::vector<pucch_collision_info> infos;
    for (unsigned r_pucch = 0; r_pucch != 16; ++r_pucch) {
      infos.emplace_back(default_res, r_pucch, bwp_cfg);
    }
    check_resources_do_not_collide_with_each_other(infos);
  }
}

TEST(pucch_collision_info_test, resources_with_non_overlapping_grants_do_not_collide)
{
  {
    // Different symbols.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(2, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_FALSE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
  {
    // Different RBs.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = pucch_constants::f2::MIN_NOF_RBS,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_FALSE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
  {
    // Different hops.
    const pucch_resource res1{.starting_prb   = 0,
                              .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .second_hop_prb = pucch_constants::f2::MIN_NOF_RBS,
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb   = pucch_constants::f2::MIN_NOF_RBS,
                              .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .second_hop_prb = 0,
                              .format_params  = pucch_resource::f1_config{}};
    ASSERT_FALSE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
}

TEST(pucch_collision_info_test, resources_with_overlapping_grants_collide)
{
  {
    // Same grants.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    ASSERT_TRUE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
  {
    // Same RBs, partially overlapping symbols.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(pucch_constants::f2::MAX_NOF_SYMS - 1,
                                                                       pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_TRUE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
  {
    // Partially overlapping RBs, same symbols.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = pucch_constants::f2::MAX_NOF_RBS - 1,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_TRUE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
  {
    // Same first hop.
    const pucch_resource res1{.starting_prb   = 0,
                              .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .second_hop_prb = 1,
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb   = 0,
                              .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .second_hop_prb = 2,
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    ASSERT_TRUE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
  {
    // Same second hop.
    const pucch_resource res1{.starting_prb   = 1,
                              .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .second_hop_prb = 0,
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb   = 2,
                              .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .second_hop_prb = 0,
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    ASSERT_TRUE(pucch_collision_info(res1, test_bwp_cfg).collides(pucch_collision_info(res2, test_bwp_cfg)));
  }
}

TEST(pucch_collision_info_test, f0_multiplexed_resources_do_not_collide)
{
  std::vector<pucch_collision_info> infos;
  for (uint8_t ics = 0; ics != pucch_constants::f0::NOF_ICS; ++ics) {
    infos.emplace_back(pucch_resource{.starting_prb = 0,
                                      .syms = ofdm_symbol_range::start_and_len(0, pucch_constants::f0::MAX_NOF_SYMS),
                                      .second_hop_prb = pucch_constants::f0::NOF_RBS,
                                      .format_params  = pucch_resource::f0_config{.initial_cyclic_shift = ics}},
                       test_bwp_cfg);
  }

  check_resources_do_not_collide_with_each_other(infos);
}

TEST(pucch_collision_info_test, f1_multiplexed_resources_do_not_collide)
{
  std::vector<pucch_collision_info> infos;
  for (uint8_t ics = 0; ics != pucch_constants::f1::NOF_ICS; ++ics) {
    for (uint8_t occ = 0; occ != pucch_constants::f1::NOF_TD_OCC; ++occ) {
      infos.emplace_back(pucch_resource{.starting_prb = 0,
                                        .syms = ofdm_symbol_range::start_and_len(0, pucch_constants::f1::MAX_NOF_SYMS),
                                        .second_hop_prb = pucch_constants::f1::NOF_RBS,
                                        .format_params  = pucch_resource::f1_config{.initial_cyclic_shift = ics,
                                                                                    .time_domain_occ      = occ}},
                         test_bwp_cfg);
    }
  }

  check_resources_do_not_collide_with_each_other(infos);
}

TEST(pucch_collision_info_test, f4_multiplexed_resources_do_not_collide)
{
  std::vector<pucch_collision_info> infos;
  for (unsigned occ = 0; occ != static_cast<unsigned>(pucch_f4_occ_len::n4); ++occ) {
    infos.emplace_back(
        pucch_resource{.starting_prb   = 0,
                       .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f1::MAX_NOF_SYMS),
                       .second_hop_prb = pucch_constants::f1::NOF_RBS,
                       .format_params  = pucch_resource::f4_config{.occ_index  = static_cast<pucch_f4_occ_idx>(occ),
                                                                   .occ_length = pucch_f4_occ_len::n4}},
        test_bwp_cfg);
  }

  check_resources_do_not_collide_with_each_other(infos);
}
