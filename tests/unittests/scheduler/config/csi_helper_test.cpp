// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
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
    const bool                               ret = csi_helper::derive_valid_csi_rs_slot_offsets(result.csi_params,
                                                                  std::nullopt,
                                                                  std::nullopt,
                                                                                                {},
                                                                  tdd_cfg,
                                                                  max_csi_symbol,
                                                                  default_ssb_period,
                                                                  default_ssb_slots,
                                                                  1U,
                                                                                                {});
    report_error_if_not(ret, "Derivation failed");
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

static csi_helper::csi_meas_config_builder_params make_csi_params_for_ports(unsigned nof_ports)
{
  static const tdd_ul_dl_config_common     tdd_cfg{subcarrier_spacing::kHz30, {10, 6, 9, 3, 0}, std::nullopt};
  static constexpr ssb_periodicity         ssb_period     = ssb_periodicity::ms10;
  static constexpr std::array<unsigned, 1> ssb_slots      = {0U};
  static constexpr std::array<unsigned, 4> track_sym      = {6, 10, 6, 10};
  const unsigned                           max_csi_symbol = *std::max_element(track_sym.begin(), track_sym.end());

  csi_helper::csi_meas_config_builder_params params{};
  params.nof_ports                = nof_ports;
  params.max_nof_layers           = std::min(nof_ports, 4U);
  params.nof_rbs                  = 52;
  params.mcs_table                = pdsch_mcs_table::qam64;
  params.csi_params.csi_rs_period = csi_helper::get_max_csi_rs_period(tdd_cfg.ref_scs);
  report_error_if_not(
      csi_helper::derive_valid_csi_rs_slot_offsets(
          params.csi_params, std::nullopt, std::nullopt, {}, tdd_cfg, max_csi_symbol, ssb_period, ssb_slots, 1U, {}),
      "Derivation failed");
  return params;
}

TEST(csi_helper_test, zp_csi_rs_resource_with_eight_ports_uses_row6)
{
  const csi_helper::csi_meas_config_builder_params params = make_csi_params_for_ports(8);

  const std::vector<zp_csi_rs_resource> zp_list = csi_helper::make_periodic_zp_csi_rs_resource_list(params);
  ASSERT_FALSE(zp_list.empty());
  const csi_rs_resource_mapping& mapping = zp_list.front().res_mapping;
  // Row 6 of TS 38.211 Table 7.4.1.5.3-1: 8 ports, 6 possible FD locations with 4 occupied, CDM group of 2.
  EXPECT_EQ(mapping.nof_ports, 8);
  EXPECT_EQ(mapping.fd_alloc.size(), 6);
  EXPECT_EQ(mapping.cdm, csi_rs_cdm_type::fd_CDM2);
  // The lowest four FD locations {0, 1, 2, 3} are occupied (fill(0, 4)).
  EXPECT_EQ(mapping.fd_alloc.count(), 4);
  for (unsigned pos = 0; pos != mapping.fd_alloc.size(); ++pos) {
    EXPECT_EQ(mapping.fd_alloc.test(pos), pos < 4) << "unexpected ZP-CSI-RS FD allocation at position " << pos;
  }
}

TEST(csi_helper_test, nzp_channel_measurement_resource_supports_eight_ports)
{
  const csi_helper::csi_meas_config_builder_params params = make_csi_params_for_ports(8);

  const std::vector<nzp_csi_rs_resource> nzp_list = csi_helper::make_nzp_csi_rs_resource_list(params);
  const auto                             cm_res   = std::find_if(
      nzp_list.begin(), nzp_list.end(), [](const nzp_csi_rs_resource& res) { return res.res_mapping.nof_ports == 8; });
  ASSERT_NE(cm_res, nzp_list.end()) << "No 8-port channel-measurement NZP-CSI-RS resource was generated";
  // 8 ports use FD-CDM2 over 6 possible FD locations, with 4 (k_hat) locations occupied.
  const csi_rs_resource_mapping& mapping = cm_res->res_mapping;
  EXPECT_EQ(mapping.fd_alloc.size(), 6);
  EXPECT_EQ(mapping.cdm, csi_rs_cdm_type::fd_CDM2);
  // The occupied locations are the four contiguous positions (pci + k_hat) mod 6, k_hat = 0..3.
  std::array<bool, 6> expected_occupied = {};
  for (unsigned i_k = 0; i_k != 4; ++i_k) {
    expected_occupied[(static_cast<unsigned>(params.pci) + i_k) % 6] = true;
  }
  EXPECT_EQ(mapping.fd_alloc.count(), 4);
  for (unsigned pos = 0; pos != mapping.fd_alloc.size(); ++pos) {
    EXPECT_EQ(mapping.fd_alloc.test(pos), expected_occupied[pos])
        << "unexpected NZP-CSI-RS FD allocation at position " << pos;
  }
}

TEST(csi_helper_test, csi_report_config_with_eight_ports_uses_four_one_codebook)
{
  const csi_helper::csi_meas_config_builder_params params = make_csi_params_for_ports(8);

  const csi_meas_config cfg = csi_helper::make_csi_meas_config(params, {});
  const auto            rep = std::find_if(cfg.csi_report_cfg_list.begin(),
                                cfg.csi_report_cfg_list.end(),
                                [](const csi_report_config& r) { return r.codebook_cfg.has_value(); });
  ASSERT_NE(rep, cfg.csi_report_cfg_list.end()) << "No CSI report with a codebook config was generated";

  ASSERT_TRUE(std::holds_alternative<codebook_config::type1>(rep->codebook_cfg->codebook_type));
  const auto& type1 = std::get<codebook_config::type1>(rep->codebook_cfg->codebook_type);
  ASSERT_TRUE(std::holds_alternative<codebook_config::type1::single_panel>(type1.sub_type));
  const auto& single_panel = std::get<codebook_config::type1::single_panel>(type1.sub_type);
  ASSERT_TRUE(std::holds_alternative<codebook_config::type1::single_panel::more_than_two_antenna_ports>(
      single_panel.nof_antenna_ports));
  const auto& port_cfg =
      std::get<codebook_config::type1::single_panel::more_than_two_antenna_ports>(single_panel.nof_antenna_ports);
  EXPECT_EQ(port_cfg.n1_n2_restriction_type, pmi_codebook_single_panel_config::four_one);
}

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

  const unsigned ssb_period_slots = to_underlying(ssb_period) * get_nof_slots_per_subframe(tdd_cfg.ref_scs);
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
