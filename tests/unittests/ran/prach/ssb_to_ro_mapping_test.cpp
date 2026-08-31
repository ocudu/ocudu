// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prach/ssb_to_ro_mapping.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::prach_helper;

namespace {

/// PRACH configuration index of TS 38.211 Table 6.3.3.2-2 with format 0 in every subframe of every system frame.
constexpr uint8_t prach_cfg_idx_every_subframe = 27;

/// PRACH configuration index of TS 38.211 Table 6.3.3.2-3 with six format A1 occasions in subframe 9 of every system
/// frame.
constexpr uint8_t prach_cfg_idx_six_occasions_in_subframe_9 = 79;

/// SS/PBCH block bitmap with L_max 8 and the blocks 0, 3, 5 and 6 active.
constexpr uint64_t ssb_bitmap_4_of_8 = 0b10010110;

rach_config_common make_rach_cfg(ssb_per_rach_occasions nof_ssb_per_ro,
                                 unsigned               msg1_fdm           = 1,
                                 uint8_t                prach_config_index = prach_cfg_idx_every_subframe)
{
  rach_config_common cfg{};
  cfg.rach_cfg_generic.prach_config_index = prach_config_index;
  cfg.rach_cfg_generic.msg1_fdm           = msg1_fdm;
  cfg.total_nof_ra_preambles              = 64;
  cfg.nof_ssb_per_ro                      = nof_ssb_per_ro;
  cfg.nof_cb_preambles_per_ssb            = ra_helper::get_preambles_per_ssb(cfg);
  cfg.msg1_scs                            = subcarrier_spacing::kHz15;
  return cfg;
}

ssb_configuration make_ssb_cfg(uint64_t bitmap, uint8_t l_max, subcarrier_spacing scs = subcarrier_spacing::kHz15)
{
  ssb_configuration cfg{};
  cfg.scs        = scs;
  cfg.ssb_period = ssb_periodicity::ms10;
  cfg.ssb_bitmap = ssb_bitmap_t(bitmap, l_max);
  return cfg;
}

/// Builds a mapping over an FDD cell where every slot of every system frame holds one PRACH occasion.
ssb_to_ro_mapping make_fdd_mapping(const rach_config_common& rach_cfg, const ssb_configuration& ssb_cfg)
{
  static const std::optional<tdd_ul_dl_config_common> no_tdd;
  return ssb_to_ro_mapping{prach_occasion_mapping_config{
      nr_band::n1, subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, rach_cfg, ssb_cfg, no_tdd}};
}

/// SS/PBCH block index of the PRACH occasion of the given system frame and slot, using preamble 0.
std::optional<unsigned> ssb_of(const ssb_to_ro_mapping& mapping, unsigned sfn, unsigned slot)
{
  const std::optional<ssb_id_t> ssb = mapping.get_ssb_index(slot_point{0, sfn, slot}, 0, 0, 0);
  return ssb.has_value() ? std::optional<unsigned>{ssb->value()} : std::nullopt;
}

/// Builds a mapping over a TDD cell with a 5msec DDDDDDDSUU pattern whose special slot holds 6 downlink and 4 uplink
/// symbols.
ssb_to_ro_mapping make_tdd_mapping(const rach_config_common& rach_cfg, const ssb_configuration& ssb_cfg)
{
  static const std::optional<tdd_ul_dl_config_common> tdd_cfg =
      tdd_ul_dl_config_common{subcarrier_spacing::kHz15, tdd_ul_dl_pattern{10, 7, 6, 2, 4}, std::nullopt};
  return ssb_to_ro_mapping{prach_occasion_mapping_config{
      nr_band::n41, subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, rach_cfg, ssb_cfg, tdd_cfg}};
}

TEST(ssb_to_ro_mapping_test, single_active_ssb_maps_every_occasion_to_it)
{
  const rach_config_common rach_cfg = make_rach_cfg(ssb_per_rach_occasions::one);
  const ssb_configuration  ssb_cfg  = make_ssb_cfg(0b00100000, 8);
  const ssb_to_ro_mapping  mapping  = make_fdd_mapping(rach_cfg, ssb_cfg);

  for (unsigned sfn = 0; sfn != 4; ++sfn) {
    for (unsigned slot = 0; slot != 10; ++slot) {
      ASSERT_EQ(ssb_of(mapping, sfn, slot), 2U);
    }
  }
}

TEST(ssb_to_ro_mapping_test, one_ssb_per_occasion_cycles_over_the_active_ssbs)
{
  const rach_config_common rach_cfg = make_rach_cfg(ssb_per_rach_occasions::one);
  const ssb_configuration  ssb_cfg  = make_ssb_cfg(ssb_bitmap_4_of_8, 8);
  const ssb_to_ro_mapping  mapping  = make_fdd_mapping(rach_cfg, ssb_cfg);

  ASSERT_EQ(mapping.association_period_frames(), 1);

  // Two complete mapping cycles of four occasions fit in the ten occasions of the association period.
  const std::array<unsigned, 8> expected = {0, 3, 5, 6, 0, 3, 5, 6};
  for (unsigned slot = 0; slot != expected.size(); ++slot) {
    ASSERT_EQ(ssb_of(mapping, 0, slot), expected[slot]) << "slot=" << slot;
  }
  // The occasions left over after the last complete cycle carry no SS/PBCH block index.
  ASSERT_FALSE(ssb_of(mapping, 0, 8).has_value());
  ASSERT_FALSE(ssb_of(mapping, 0, 9).has_value());

  // The association period is one system frame, so the pattern repeats every frame.
  ASSERT_EQ(ssb_of(mapping, 1, 0), 0U);
  ASSERT_EQ(ssb_of(mapping, 7, 3), 6U);
}

TEST(ssb_to_ro_mapping_test, half_ssb_per_occasion_maps_one_ssb_to_two_occasions)
{
  const rach_config_common rach_cfg = make_rach_cfg(ssb_per_rach_occasions::one_half);
  const ssb_configuration  ssb_cfg  = make_ssb_cfg(ssb_bitmap_4_of_8, 8);
  const ssb_to_ro_mapping  mapping  = make_fdd_mapping(rach_cfg, ssb_cfg);

  ASSERT_EQ(mapping.association_period_frames(), 1);

  const std::array<unsigned, 8> expected = {0, 0, 3, 3, 5, 5, 6, 6};
  for (unsigned slot = 0; slot != expected.size(); ++slot) {
    ASSERT_EQ(ssb_of(mapping, 0, slot), expected[slot]) << "slot=" << slot;
  }
  ASSERT_FALSE(ssb_of(mapping, 0, 8).has_value());
  ASSERT_FALSE(ssb_of(mapping, 0, 9).has_value());
}

TEST(ssb_to_ro_mapping_test, association_period_grows_until_every_ssb_is_mapped)
{
  const rach_config_common rach_cfg = make_rach_cfg(ssb_per_rach_occasions::one_forth);
  const ssb_configuration  ssb_cfg  = make_ssb_cfg(ssb_bitmap_4_of_8, 8);
  const ssb_to_ro_mapping  mapping  = make_fdd_mapping(rach_cfg, ssb_cfg);

  // Sixteen occasions are needed to map the four active SS/PBCH blocks, so a single ten-occasion system frame is not
  // enough.
  ASSERT_EQ(mapping.association_period_frames(), 2);

  const std::array<unsigned, 16> expected = {0, 0, 0, 0, 3, 3, 3, 3, 5, 5, 5, 5, 6, 6, 6, 6};
  for (unsigned ro = 0; ro != expected.size(); ++ro) {
    ASSERT_EQ(ssb_of(mapping, ro / 10, ro % 10), expected[ro]) << "ro=" << ro;
  }
  for (unsigned ro = 16; ro != 20; ++ro) {
    ASSERT_FALSE(ssb_of(mapping, ro / 10, ro % 10).has_value()) << "ro=" << ro;
  }
  // Occasion ordinals restart at every association period.
  ASSERT_EQ(ssb_of(mapping, 2, 0), 0U);
}

TEST(ssb_to_ro_mapping_test, time_multiplexed_occasions_are_ordered_within_the_prach_slot)
{
  const rach_config_common rach_cfg =
      make_rach_cfg(ssb_per_rach_occasions::one, 1, prach_cfg_idx_six_occasions_in_subframe_9);
  const ssb_configuration ssb_cfg = make_ssb_cfg(ssb_bitmap_4_of_8, 8);
  const ssb_to_ro_mapping mapping = make_tdd_mapping(rach_cfg, ssb_cfg);

  // The six time-domain occasions of slot 9 all fall within uplink symbols, so the first four map the active
  // SS/PBCH blocks and the last two are left over.
  const std::array<unsigned, 4> expected = {0, 3, 5, 6};
  for (unsigned td = 0; td != expected.size(); ++td) {
    const std::optional<ssb_id_t> ssb = mapping.get_ssb_index(slot_point{0, 0, 9}, td, 0, 0);
    ASSERT_TRUE(ssb.has_value()) << "td=" << td;
    ASSERT_EQ(ssb->value(), expected[td]) << "td=" << td;
  }
  ASSERT_FALSE(mapping.get_ssb_index(slot_point{0, 0, 9}, 4, 0, 0).has_value());
  ASSERT_FALSE(mapping.get_ssb_index(slot_point{0, 0, 9}, 5, 0, 0).has_value());
  // Slots that hold no PRACH occasion carry no SS/PBCH block index.
  ASSERT_FALSE(mapping.get_ssb_index(slot_point{0, 0, 8}, 0, 0, 0).has_value());
}

TEST(ssb_to_ro_mapping_test, frequency_multiplexed_occasions_are_ordered_first)
{
  const rach_config_common rach_cfg = make_rach_cfg(ssb_per_rach_occasions::one, 4);
  const ssb_configuration  ssb_cfg  = make_ssb_cfg(ssb_bitmap_4_of_8, 8);
  const ssb_to_ro_mapping  mapping  = make_fdd_mapping(rach_cfg, ssb_cfg);

  // The four frequency-multiplexed occasions of the first PRACH slot are consecutive in the occasion ordering.
  const std::array<unsigned, 4> expected = {0, 3, 5, 6};
  for (unsigned fd = 0; fd != expected.size(); ++fd) {
    const std::optional<ssb_id_t> ssb = mapping.get_ssb_index(slot_point{0, 0, 0}, 0, fd, 0);
    ASSERT_TRUE(ssb.has_value());
    ASSERT_EQ(ssb->value(), expected[fd]) << "fd=" << fd;
  }
}

} // namespace
