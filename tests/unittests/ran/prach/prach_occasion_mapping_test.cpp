// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/prach/prach_occasion_mapping.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::prach_helper;

namespace {

/// PRACH configuration index of TS 38.211 Table 6.3.3.2-2 with format 0 in every subframe of every system frame.
constexpr uint8_t prach_cfg_idx_every_subframe = 27;

/// PRACH configuration index of TS 38.211 Table 6.3.3.2-3 with six format A1 occasions in subframe 9 of every system
/// frame.
constexpr uint8_t prach_cfg_idx_six_occasions_in_subframe_9 = 79;

rach_config_common make_rach_cfg(uint8_t prach_config_index)
{
  rach_config_common cfg{};
  cfg.rach_cfg_generic.prach_config_index = prach_config_index;
  cfg.rach_cfg_generic.msg1_fdm           = 1;
  cfg.total_nof_ra_preambles              = 64;
  cfg.nof_ssb_per_ro                      = ssb_per_rach_occasions::one;
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

/// Builds a mapping over an FDD cell, where every PRACH occasion is valid.
prach_occasion_mapping make_fdd_mapping(const rach_config_common& rach_cfg, const ssb_configuration& ssb_cfg)
{
  static const std::optional<tdd_ul_dl_config_common> no_tdd;
  return prach_occasion_mapping{prach_occasion_mapping_config{
      nr_band::n1, subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, rach_cfg, ssb_cfg, no_tdd}};
}

/// Builds a mapping over a TDD cell with a 5msec DDDDDDDSUU pattern whose special slot holds 6 downlink and 4 uplink
/// symbols.
prach_occasion_mapping make_tdd_mapping(const rach_config_common& rach_cfg, const ssb_configuration& ssb_cfg)
{
  static const std::optional<tdd_ul_dl_config_common> tdd_cfg =
      tdd_ul_dl_config_common{subcarrier_spacing::kHz15, tdd_ul_dl_pattern{10, 7, 6, 2, 4}, std::nullopt};
  return prach_occasion_mapping{prach_occasion_mapping_config{
      nr_band::n41, subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, rach_cfg, ssb_cfg, tdd_cfg}};
}

TEST(prach_occasion_mapping_test, all_occasions_of_a_paired_spectrum_cell_are_valid)
{
  const rach_config_common     rach_cfg = make_rach_cfg(prach_cfg_idx_every_subframe);
  const ssb_configuration      ssb_cfg  = make_ssb_cfg(0b10010110, 8);
  const prach_occasion_mapping mapping  = make_fdd_mapping(rach_cfg, ssb_cfg);

  ASSERT_TRUE(mapping.is_valid_ro(slot_point{0, 0, 0}, ofdm_symbol_range{0, 14}));
  ASSERT_TRUE(mapping.is_valid_ro(slot_point{0, 3, 7}, ofdm_symbol_range{2, 6}));
}

TEST(prach_occasion_mapping_test, occasion_within_uplink_symbols_is_valid)
{
  const rach_config_common     rach_cfg = make_rach_cfg(prach_cfg_idx_six_occasions_in_subframe_9);
  const ssb_configuration      ssb_cfg  = make_ssb_cfg(0b10000000, 8);
  const prach_occasion_mapping mapping  = make_tdd_mapping(rach_cfg, ssb_cfg);

  // Fully uplink slots.
  ASSERT_TRUE(mapping.is_valid_ro(slot_point{0, 0, 8}, ofdm_symbol_range{0, 14}));
  ASSERT_TRUE(mapping.is_valid_ro(slot_point{0, 0, 9}, ofdm_symbol_range{4, 8}));
  // Uplink symbols of the special slot.
  ASSERT_TRUE(mapping.is_valid_ro(slot_point{0, 0, 7}, ofdm_symbol_range{10, 14}));
}

TEST(prach_occasion_mapping_test, occasion_in_a_special_slot_is_valid_only_after_the_gap)
{
  const rach_config_common rach_cfg = make_rach_cfg(prach_cfg_idx_six_occasions_in_subframe_9);
  // SS/PBCH block 0 occupies symbols 2 to 5 of slot 0 in pattern case A.
  const ssb_configuration      ssb_cfg = make_ssb_cfg(0b10000000, 8);
  const prach_occasion_mapping mapping = make_tdd_mapping(rach_cfg, ssb_cfg);

  // The special slot holds 6 downlink symbols, so with N_gap of 2 an occasion is valid from symbol 8 onwards.
  ASSERT_FALSE(mapping.is_valid_ro(slot_point{0, 0, 7}, ofdm_symbol_range{6, 10}));
  ASSERT_FALSE(mapping.is_valid_ro(slot_point{0, 0, 7}, ofdm_symbol_range{7, 11}));
  ASSERT_TRUE(mapping.is_valid_ro(slot_point{0, 0, 7}, ofdm_symbol_range{8, 12}));

  // A fully downlink slot never holds a valid occasion.
  ASSERT_FALSE(mapping.is_valid_ro(slot_point{0, 0, 0}, ofdm_symbol_range{8, 12}));
}

TEST(prach_occasion_mapping_test, ssb_symbols_are_rescaled_to_the_uplink_numerology)
{
  // SS/PBCH block 0 of pattern case A spans the 15kHz symbols 2 to 5, i.e. the 30kHz symbols 4 to 11 of slot 0.
  const rach_config_common rach_cfg = make_rach_cfg(prach_cfg_idx_six_occasions_in_subframe_9);
  const ssb_configuration  ssb_cfg  = make_ssb_cfg(0b10000000, 8, subcarrier_spacing::kHz15);

  static const std::optional<tdd_ul_dl_config_common> tdd_cfg =
      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, tdd_ul_dl_pattern{20, 0, 0, 19, 0}, std::nullopt};
  const prach_occasion_mapping mapping{prach_occasion_mapping_config{
      nr_band::n41, subcarrier_spacing::kHz30, cyclic_prefix::NORMAL, rach_cfg, ssb_cfg, tdd_cfg}};

  // Slot 0 holds no uplink symbol and its last SS/PBCH block symbol is 11, so with N_gap of 2 no occasion fits.
  ASSERT_FALSE(mapping.is_valid_ro(slot_point{1, 0, 0}, ofdm_symbol_range{12, 14}));
  // Slot 1 holds no SS/PBCH block and is fully uplink.
  ASSERT_TRUE(mapping.is_valid_ro(slot_point{1, 0, 1}, ofdm_symbol_range{0, 14}));
}

TEST(prach_occasion_mapping_test, only_the_configured_slots_start_a_prach_burst)
{
  const rach_config_common     rach_cfg = make_rach_cfg(prach_cfg_idx_six_occasions_in_subframe_9);
  const ssb_configuration      ssb_cfg  = make_ssb_cfg(0b10000000, 8);
  const prach_occasion_mapping mapping  = make_tdd_mapping(rach_cfg, ssb_cfg);

  // The occasions of this configuration are placed in subframe 9 only.
  ASSERT_TRUE(mapping.is_valid_prach_slot(slot_point{0, 0, 9}));
  ASSERT_FALSE(mapping.is_valid_prach_slot(slot_point{0, 0, 8}));
}

} // namespace
