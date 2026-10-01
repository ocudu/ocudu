// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/support/pucch/pucch_collision.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/span.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

static void check_resources_do_not_collide_with_each_other(span<const pucch_resource> resources)
{
  for (unsigned i = 0; i != resources.size(); ++i) {
    for (unsigned j = 0; j != resources.size(); ++j) {
      if (i == j) {
        ASSERT_TRUE(pucch_resources_collide(resources[i], resources[j]));
      } else {
        ASSERT_FALSE(pucch_resources_collide(resources[i], resources[j]));
      }
    }
  }
}

TEST(pucch_collision_info_test, common_resources_do_not_collide)
{
  static constexpr unsigned n_bwp_size = 25;

  for (unsigned row_index = 0; row_index != 16; ++row_index) {
    auto resources = config_helpers::generate_cell_common_pucch_res_list(row_index, n_bwp_size);
    check_resources_do_not_collide_with_each_other(resources);
  }
}

TEST(pucch_resource_collides_test, resources_with_non_overlapping_grants_do_not_collide)
{
  {
    // Different symbols.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(2, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_FALSE(pucch_resources_collide(res1, res2));
  }
  {
    // Different RBs.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MIN_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = pucch_constants::f2::MIN_NOF_RBS,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_FALSE(pucch_resources_collide(res1, res2));
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
    ASSERT_FALSE(pucch_resources_collide(res1, res2));
  }
}

TEST(pucch_resource_collides_test, resources_with_overlapping_grants_collide)
{
  {
    // Same grants.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    ASSERT_TRUE(pucch_resources_collide(res1, res2));
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
    ASSERT_TRUE(pucch_resources_collide(res1, res2));
  }
  {
    // Partially overlapping RBs, same symbols.
    const pucch_resource res1{.starting_prb  = 0,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f2_config{.nof_prbs = pucch_constants::f2::MAX_NOF_RBS}};
    const pucch_resource res2{.starting_prb  = pucch_constants::f2::MAX_NOF_RBS - 1,
                              .syms          = ofdm_symbol_range::start_and_len(0, pucch_constants::f2::MAX_NOF_SYMS),
                              .format_params = pucch_resource::f1_config{}};
    ASSERT_TRUE(pucch_resources_collide(res1, res2));
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
    ASSERT_TRUE(pucch_resources_collide(res1, res2));
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
    ASSERT_TRUE(pucch_resources_collide(res1, res2));
  }
}

TEST(pucch_resource_collides_test, f0_multiplexed_resources_do_not_collide)
{
  std::vector<pucch_resource> resources;
  for (uint8_t ics = 0; ics != pucch_constants::f0::NOF_ICS; ++ics) {
    resources.push_back(pucch_resource{.starting_prb = 0,
                                       .syms = ofdm_symbol_range::start_and_len(0, pucch_constants::f0::MAX_NOF_SYMS),
                                       .second_hop_prb = pucch_constants::f0::NOF_RBS,
                                       .format_params  = pucch_resource::f0_config{.initial_cyclic_shift = ics}});
  }
  check_resources_do_not_collide_with_each_other(resources);
}

TEST(pucch_resource_collides_test, f1_multiplexed_resources_do_not_collide)
{
  std::vector<pucch_resource> resources;
  for (uint8_t ics = 0; ics != pucch_constants::f1::NOF_ICS; ++ics) {
    for (uint8_t occ = 0; occ != pucch_constants::f1::NOF_TD_OCC; ++occ) {
      resources.push_back(pucch_resource{
          .starting_prb   = 0,
          .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f1::MAX_NOF_SYMS),
          .second_hop_prb = pucch_constants::f1::NOF_RBS,
          .format_params  = pucch_resource::f1_config{.initial_cyclic_shift = ics, .time_domain_occ = occ}});
    }
  }
  check_resources_do_not_collide_with_each_other(resources);
}

TEST(pucch_resource_collides_test, f4_multiplexed_resources_do_not_collide)
{
  std::vector<pucch_resource> resources;
  for (unsigned occ = 0; occ != static_cast<unsigned>(pucch_f4_occ_len::n4); ++occ) {
    resources.push_back(
        pucch_resource{.starting_prb   = 0,
                       .syms           = ofdm_symbol_range::start_and_len(0, pucch_constants::f1::MAX_NOF_SYMS),
                       .second_hop_prb = pucch_constants::f1::NOF_RBS,
                       .format_params  = pucch_resource::f4_config{.occ_index  = static_cast<pucch_f4_occ_idx>(occ),
                                                                   .occ_length = pucch_f4_occ_len::n4}});
  }
  check_resources_do_not_collide_with_each_other(resources);
}
