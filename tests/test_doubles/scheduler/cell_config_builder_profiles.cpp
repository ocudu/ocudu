// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_config_builder_profiles.h"
#include "ocudu/ran/band_helper.h"

using namespace ocudu;

/// Create cell build parameters for a TDD band.
static cell_config_builder_params tdd(const std::optional<bs_channel_bandwidth>& bw)
{
  cell_config_builder_params params{};
  params.scs_common             = subcarrier_spacing::kHz30;
  params.dl_carrier.arfcn_f_ref = 520002;
  params.dl_carrier.band        = band_helper::get_band_from_dl_arfcn(params.dl_carrier.arfcn_f_ref);
  params.dl_carrier.carrier_bw  = bw.value_or(bs_channel_bandwidth::MHz20);
  return params;
}

/// Create cell build parameters for a FDD band.
static cell_config_builder_params fdd(const std::optional<bs_channel_bandwidth>& bw)
{
  cell_config_builder_params params{};
  params.scs_common             = subcarrier_spacing::kHz15;
  params.dl_carrier.arfcn_f_ref = 530000;
  params.dl_carrier.band        = band_helper::get_band_from_dl_arfcn(params.dl_carrier.arfcn_f_ref);
  params.dl_carrier.carrier_bw  = bw.value_or(bs_channel_bandwidth::MHz20);
  return params;
}

/// Create cell build parameters for a TDD FR2 band.
static cell_config_builder_params tdd_fr2(const std::optional<bs_channel_bandwidth>& bw)
{
  cell_config_builder_params params{};
  params.scs_common             = subcarrier_spacing::kHz120;
  params.dl_carrier.arfcn_f_ref = 2074171;
  params.dl_carrier.band        = band_helper::get_band_from_dl_arfcn(params.dl_carrier.arfcn_f_ref);
  params.dl_carrier.carrier_bw  = bw.value_or(bs_channel_bandwidth::MHz100);
  return params;
}

cell_config_builder_params cell_config_builder_profiles::create(duplex_mode                                mode,
                                                                frequency_range                            fr,
                                                                const std::optional<bs_channel_bandwidth>& bw)
{
  if (mode == duplex_mode::TDD) {
    if (fr == frequency_range::FR1) {
      return tdd(bw);
    }
    return tdd_fr2(bw);
  }
  report_error_if_not(fr == frequency_range::FR1, "FDD bands are only supported in FR1");
  return fdd(bw);
}

cell_config_builder_params cell_config_builder_profiles::create(nr_band band)
{
  cell_config_builder_params params;
  params.dl_carrier.band = band;
  switch (band) {
    case nr_band::n3:
      params.scs_common             = subcarrier_spacing::kHz30;
      params.dl_carrier.arfcn_f_ref = 361000;
      params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz20;
      break;
    case nr_band::n7:
      params.scs_common             = subcarrier_spacing::kHz15;
      params.dl_carrier.arfcn_f_ref = 530000;
      params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz20;
      break;
    case nr_band::n41:
      // TDD FR1, 2496–2690 MHz.
      params.scs_common             = subcarrier_spacing::kHz30;
      params.dl_carrier.arfcn_f_ref = 520002;
      params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz20;
      break;
    case nr_band::n261:
      // TDD FR2, 27500–28350 MHz.
      params.scs_common             = subcarrier_spacing::kHz120;
      params.dl_carrier.arfcn_f_ref = 2074171;
      params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz100;
      break;
    default:
      report_fatal_error("Band {} to test NR-ARFCN lookup not yet supported. Add a new entry",
                         static_cast<unsigned>(band));
  }
  ocudu_assert(
      band_helper::is_dl_arfcn_valid_given_band(
          params.dl_carrier.band, params.dl_carrier.arfcn_f_ref, params.scs_common, params.dl_carrier.carrier_bw),
      "Invalid cell config profile for band={}",
      static_cast<unsigned>(params.dl_carrier.band));
  return params;
}

tdd_ul_dl_config_common cell_config_builder_profiles::create_tdd_pattern(tdd_pattern_profile_fr1_30khz pattern)
{
  tdd_ul_dl_config_common cfg;
  cfg.ref_scs = subcarrier_spacing::kHz30;

  switch (pattern) {
    case tdd_pattern_profile_fr1_30khz::DDDDDDDSUU:
      // FR1.30-1.
      cfg.pattern1 = {10, 7, 6, 2, 4};
      break;
    case tdd_pattern_profile_fr1_30khz::DDDSU:
      // FR1.30-2.
      cfg.pattern1 = {5, 3, 10, 1, 2};
      break;
    case tdd_pattern_profile_fr1_30khz::DDDSUDDSUU:
      // FR1.30-3.
      cfg.pattern1 = {5, 3, 10, 1, 2};
      cfg.pattern2 = {5, 2, 10, 2, 2};
      break;
    case tdd_pattern_profile_fr1_30khz::DDDSUUDDDD:
      // FR1.30-4.
      cfg.pattern1 = {6, 3, 6, 2, 4};
      cfg.pattern2 = {4, 4, 0, 0, 0};
      break;
    case tdd_pattern_profile_fr1_30khz::DSUU:
      // FR1.30-5.
      cfg.pattern1 = {4, 1, 12, 2, 0};
      break;
    case tdd_pattern_profile_fr1_30khz::DSSU:
      // FR1.30-6.
      cfg.pattern1 = {2, 1, 10, 0, 2};
      cfg.pattern2 = {2, 0, 12, 1, 0};
      break;
    case tdd_pattern_profile_fr1_30khz::DDDDDDSUUU:
      // custom.
      cfg.pattern1 = {10, 6, 5, 3, 4};
      break;
    default:
      report_fatal_error("Unrecognized pattern profile");
  }
  return cfg;
}

tdd_ul_dl_config_common cell_config_builder_profiles::create_tdd_pattern(tdd_pattern_profile_fr2_60khz pattern)
{
  tdd_ul_dl_config_common cfg;
  cfg.ref_scs = subcarrier_spacing::kHz60;
  switch (pattern) {
    case tdd_pattern_profile_fr2_60khz::DDSU:
      cfg.pattern1 = {4, 2, 11, 1, 0};
      break;
    default:
      report_fatal_error("Unrecognized pattern profile");
  }
  return cfg;
}

tdd_ul_dl_config_common cell_config_builder_profiles::create_tdd_pattern(tdd_pattern_profile_fr2_120khz pattern)
{
  tdd_ul_dl_config_common cfg;
  cfg.ref_scs = subcarrier_spacing::kHz120;
  switch (pattern) {
    case tdd_pattern_profile_fr2_120khz::DDDSU:
      cfg.pattern1 = {5, 3, 10, 1, 2};
      break;
    case tdd_pattern_profile_fr2_120khz::DDSU:
      cfg.pattern1 = {4, 2, 11, 1, 0};
      break;
    default:
      report_fatal_error("Unrecognized pattern profile");
  }
  return cfg;
}
