// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/bs_channel_bandwidth.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <array>
#include <limits>

namespace ocudu {

// Tags used to differentiate RB types.
struct crb_tag {};
struct prb_tag {};
struct vrb_tag {};

/// Maximum number of PRBs of a UE carrier, for any SCS.
/// \remark See TS 38.331 - "maxNrofPhysicalResourceBlocks".
/// \remark It can be set to any custom value using the pre-processor definition \c MAX_NPRB.
#ifdef MAX_NPRB
constexpr std::size_t MAX_NOF_PRBS = MAX_NPRB;
#else
constexpr std::size_t MAX_NOF_PRBS = 275;
#endif // MAX_NPRB

/// See TS 38.211, 4.4.4.1 - Resource Blocks, General.
constexpr std::size_t NOF_SUBCARRIERS_PER_RB = 12;

/// Maximum number of subcarriers in a carrier.
/// \remark See MAX_NOF_PRBS and NOF_SUBCARRIERS_PER_RB.
constexpr std::size_t MAX_NOF_SUBCARRIERS = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB;

/// Compute RB bandwidth in kHz, based on the used numerology.
constexpr std::size_t get_rb_bw_khz(subcarrier_spacing scs)
{
  return NOF_SUBCARRIERS_PER_RB * scs_to_khz(scs);
}

/// Converts channel bandwidth in MHz to index.
constexpr std::size_t channel_bw_mhz_to_index_fr1(bs_channel_bandwidth dl_bw_mhz)
{
  switch (dl_bw_mhz) {
    case bs_channel_bandwidth::MHz5:
      return 0;
    case bs_channel_bandwidth::MHz10:
      return 1;
    case bs_channel_bandwidth::MHz15:
      return 2;
    case bs_channel_bandwidth::MHz20:
      return 3;
    case bs_channel_bandwidth::MHz25:
      return 4;
    case bs_channel_bandwidth::MHz30:
      return 5;
    case bs_channel_bandwidth::MHz40:
      return 6;
    case bs_channel_bandwidth::MHz50:
      return 7;
    case bs_channel_bandwidth::MHz60:
      return 8;
    case bs_channel_bandwidth::MHz70:
      return 9;
    case bs_channel_bandwidth::MHz80:
      return 10;
    case bs_channel_bandwidth::MHz90:
      return 11;
    case bs_channel_bandwidth::MHz100:
      return 12;
    default:
      break;
  }
  return std::numeric_limits<size_t>::max();
}

/// Converts channel bandwidth in MHz to index.
constexpr std::size_t channel_bw_mhz_to_index_fr2(bs_channel_bandwidth dl_bw_mhz)
{
  switch (dl_bw_mhz) {
    case bs_channel_bandwidth::MHz50:
      return 0;
    case bs_channel_bandwidth::MHz100:
      return 1;
    case bs_channel_bandwidth::MHz200:
      return 2;
    case bs_channel_bandwidth::MHz400:
      return 3;
    default:
      break;
  }
  return std::numeric_limits<size_t>::max();
}

/// Computes the maximum transmission bandwidth in number of PRBs.
/// \remark See TS 38.104, Table 5.3.2-1/2 - Transmission bandwidth configuration for FR1/FR2.
inline std::size_t get_max_Nprb(bs_channel_bandwidth dl_bw_mhz, subcarrier_spacing scs, frequency_range fr)
{
  ocudu_sanity_check(is_scs_valid(scs, fr), "Invalid SCS");
  static constexpr std::array<std::size_t, 3> rb_table_fr1[] = {
      // clang-format off
      // SCS
      // 15,  30,  60.
      {  25,  11,   0}, // 5 MHz.
      {  52,  24,  11}, // 10 MHz.
      {  79,  38,  18}, // 15 MHz.
      { 106,  51,  24}, // 20 MHz.
      { 133,  65,  31}, // 25 MHz.
      { 160,  78,  38}, // 30 MHz.
      { 216, 106,  51}, // 40 MHz.
      { 270, 133,  65}, // 50 MHz.
      {   0, 162,  79}, // 60 MHz.
      {   0, 189,  93}, // 70 MHz.
      {   0, 217, 107}, // 80 MHz.
      {   0, 245, 121}, // 90 MHz.
      {   0, 273, 135}, // 100 MHz.
      // clang-format on
  };
  static constexpr std::array<std::size_t, 2> rb_table_fr2[] = {
      // clang-format off
      // SCS
      // 60, 120.
      {  66,  32}, // 50 MHz.
      { 132,  66}, // 100 MHz.
      { 264, 132}, // 200 MHz.
      {   0, 264}, // 400 MHz.
      // clang-format on
  };

  size_t bw_idx;
  if (fr == frequency_range::FR1) {
    bw_idx = channel_bw_mhz_to_index_fr1(dl_bw_mhz);
    if (bw_idx == std::numeric_limits<size_t>::max()) {
      return 0;
    }
    return rb_table_fr1[bw_idx][to_numerology_value(scs)];
  }

  bw_idx = channel_bw_mhz_to_index_fr2(dl_bw_mhz);
  if (bw_idx == std::numeric_limits<size_t>::max()) {
    return 0;
  }
  return rb_table_fr2[bw_idx][to_numerology_value(scs) - to_numerology_value(subcarrier_spacing::kHz60)];
}

} // namespace ocudu
