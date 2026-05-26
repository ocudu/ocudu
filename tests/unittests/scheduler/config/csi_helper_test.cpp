// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/tdd/tdd_ul_dl_config_formatters.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/config/serving_cell_config_validator.h"
#include "ocudu/support/enum_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace ocudu {

void PrintTo(const tdd_ul_dl_config_common& cfg, std::ostream* os)
{
  *os << fmt::format("{}", cfg);
}

} // namespace ocudu

class csi_rs_slot_derivation_test : public ::testing::TestWithParam<tdd_ul_dl_config_common>
{
protected:
  csi_rs_slot_derivation_test()
  {
    static constexpr std::array<unsigned, 4> def_track_csi_ofdm_symbol_idx = {6, 10, 6, 10};
    const unsigned                           max_csi_symbol =
        *std::max_element(def_track_csi_ofdm_symbol_idx.begin(), def_track_csi_ofdm_symbol_idx.end());
    static constexpr ssb_periodicity         default_ssb_period = ssb_periodicity::ms10;
    static constexpr std::array<unsigned, 1> default_ssb_slots  = {0U};
    ocudu_assert(csi_helper::derive_valid_csi_rs_slot_offsets(result.csi_params,
                                                              std::nullopt,
                                                              std::nullopt,
                                                              {},
                                                              tdd_cfg,
                                                              max_csi_symbol,
                                                              default_ssb_period,
                                                              default_ssb_slots,
                                                              1U,
                                                              {}),
                 "Derivation failed");
  }

  tdd_ul_dl_config_common                    tdd_cfg = GetParam();
  csi_helper::csi_meas_config_builder_params result{};
};

TEST_P(csi_rs_slot_derivation_test, csi_rs_slot_offset_fall_in_dl_slots)
{
  static const unsigned ZP_SYMBOL_IDX = 8, MEAS_SYMBOL_IDX = 4, TRACKING_MAX_SYMBOL_IDX = 8;

  ASSERT_GE(get_active_tdd_dl_symbols(tdd_cfg, result.csi_params.zp_csi_slot_offsets[0], cyclic_prefix::NORMAL).stop(),
            ZP_SYMBOL_IDX);
  ASSERT_GE(
      get_active_tdd_dl_symbols(tdd_cfg, result.csi_params.meas_csi_slot_offsets[0], cyclic_prefix::NORMAL).stop(),
      MEAS_SYMBOL_IDX);
  // Note: Tracking occupies two consecutive slots.
  ASSERT_GE(
      get_active_tdd_dl_symbols(tdd_cfg, result.csi_params.tracking_csi_slot_offset, cyclic_prefix::NORMAL).stop(),
      TRACKING_MAX_SYMBOL_IDX);
  ASSERT_GE(
      get_active_tdd_dl_symbols(tdd_cfg, result.csi_params.tracking_csi_slot_offset + 1, cyclic_prefix::NORMAL).stop(),
      TRACKING_MAX_SYMBOL_IDX);
}

TEST_P(csi_rs_slot_derivation_test, csi_rs_slot_offsets_do_not_collide)
{
  // Note: ZP and NZP-CSI-RS slots are always in different symbols.
  ASSERT_NE(result.csi_params.zp_csi_slot_offsets[0], result.csi_params.tracking_csi_slot_offset);
  ASSERT_NE(result.csi_params.zp_csi_slot_offsets[0], result.csi_params.tracking_csi_slot_offset + 1);
  ASSERT_NE(result.csi_params.meas_csi_slot_offsets[0], result.csi_params.tracking_csi_slot_offset);
  ASSERT_NE(result.csi_params.meas_csi_slot_offsets[0], result.csi_params.tracking_csi_slot_offset + 1);
}

TEST_P(csi_rs_slot_derivation_test, generated_csi_meas_config_validation)
{
  serving_cell_config cell_cfg =
      config_helpers::make_default_ue_cell_config(config_helpers::make_default_ran_cell_config()).serv_cell_cfg;
  result.nof_rbs   = 52;
  result.mcs_table = pdsch_mcs_table::qam64;
  // Note: Since by default we use periodic CSI, we don't care about pusch_td_alloc_list or ul_config_common.
  cell_cfg.csi_meas_cfg = make_csi_meas_config(result, {});
  ul_config_common ul_cfg_cmn{};
  config_validators::validate_csi_meas_cfg(cell_cfg, tdd_cfg, ul_cfg_cmn);
}

INSTANTIATE_TEST_SUITE_P(
    csi_helper_test,
    csi_rs_slot_derivation_test,
    // clang-format off
    ::testing::Values(tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}, std::nullopt},
                      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 9, 3, 0}, std::nullopt},
                      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 9, 2, 0}, std::nullopt}));
// clang-format on

TEST(csi_helper_test, ssb_slot_offsets_are_all_avoided)
{
  // With L_max=4 and all beams active, SSBs occupy slots 0 and 1 (for 30 kHz SCS, case C).
  // Verify that no derived CSI-RS offset lands on either of those slots within the SSB period.
  static const tdd_ul_dl_config_common     tdd_cfg{subcarrier_spacing::kHz30, {10, 6, 9, 3, 0}, std::nullopt};
  static constexpr ssb_periodicity         ssb_period = ssb_periodicity::ms10;
  static constexpr std::array<unsigned, 2> ssb_slots  = {0U, 1U};

  static constexpr std::array<unsigned, 4> track_sym      = {6, 10, 6, 10};
  const unsigned                           max_csi_symbol = *std::max_element(track_sym.begin(), track_sym.end());

  csi_helper::csi_meas_config_builder_params params{};
  params.csi_params.csi_rs_period = csi_helper::get_max_csi_rs_period(tdd_cfg.ref_scs);

  ASSERT_TRUE(csi_helper::derive_valid_csi_rs_slot_offsets(
      params.csi_params, std::nullopt, std::nullopt, {}, tdd_cfg, max_csi_symbol, ssb_period, ssb_slots, 1U, {}));

  const unsigned ssb_period_slots = to_value(ssb_period) * get_nof_slots_per_subframe(tdd_cfg.ref_scs);
  for (unsigned ssb_slot : ssb_slots) {
    EXPECT_NE(params.csi_params.meas_csi_slot_offsets[0] % ssb_period_slots, ssb_slot)
        << "meas_csi_slot_offsets[0] collides with SSB slot " << ssb_slot;
    EXPECT_NE(params.csi_params.zp_csi_slot_offsets[0] % ssb_period_slots, ssb_slot)
        << "zp_csi_slot_offsets[0] collides with SSB slot " << ssb_slot;
    EXPECT_NE(params.csi_params.tracking_csi_slot_offset % ssb_period_slots, ssb_slot)
        << "tracking_csi_slot_offset collides with SSB slot " << ssb_slot;
    EXPECT_NE((params.csi_params.tracking_csi_slot_offset + 1) % ssb_period_slots, ssb_slot)
        << "tracking_csi_slot_offset+1 collides with SSB slot " << ssb_slot;
  }
}
