// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/duplex_mode.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"

namespace ocudu {
namespace cell_config_builder_profiles {

/// Create basic cell build parameters with given duplex mode, frequency range and bandwidth.
cell_config_builder_params create(duplex_mode                                mode = duplex_mode::TDD,
                                  frequency_range                            fr   = frequency_range::FR1,
                                  const std::optional<bs_channel_bandwidth>& bw   = std::nullopt);

/// Create basic cell build parameters with given band and bandwidth.
cell_config_builder_params create(nr_band band);

/// List of TDD UL-DL configurations for FR1, specified in TS 38.101-4, Table A.1.2-2.
enum class tdd_pattern_profile_fr1_30khz {
  DDDDDDDSUU, ///< FR1.30-1.
  DDDSU,      ///< FR1.30-2.
  DDDSUDDSUU, ///< FR1.30-3.
  DDDSUUDDDD, ///< FR1.30-4.
  DSUU,       ///< FR1.30-5.
  DSSU,       ///< FR1.30-6.
  DDDDDDSUUU  ///< custom.
};

/// List of TDD UL-DL configurations for FR2, SCS 60kHz, specified in TS 38.101-4, Table A.1.3-1.
enum class tdd_pattern_profile_fr2_60khz {
  DDSU /// < FR2.60-1.
};

/// List of TDD UL-DL configurations for FR2, SCS 120kHz, specified in TS 38.101-4, Table A.1.3-2.
enum class tdd_pattern_profile_fr2_120khz {
  DDDSU, /// < FR2.120-1.
  DDSU   /// < FR2.120-2.
};

tdd_ul_dl_config_common create_tdd_pattern(tdd_pattern_profile_fr1_30khz pattern);
tdd_ul_dl_config_common create_tdd_pattern(tdd_pattern_profile_fr2_60khz pattern);
tdd_ul_dl_config_common create_tdd_pattern(tdd_pattern_profile_fr2_120khz pattern);

} // namespace cell_config_builder_profiles
} // namespace ocudu
