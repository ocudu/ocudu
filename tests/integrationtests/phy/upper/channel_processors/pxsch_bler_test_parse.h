// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/channel_estimator/port_channel_estimator_parameters.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/pusch/pusch_mcs.h"
#include "ocudu/ran/sch/sch_mcs.h"
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace ocudu {

/// Configuration parameters parsed from CLI11.
struct pxsch_bler_test_configuration {
  unsigned                                         max_nof_threads = std::min(8U, std::thread::hardware_concurrency());
  bool                                             show_stats      = true;
  unsigned                                         nof_repetitions = 1000;
  std::string                                      channel_delay_profile            = "single-tap";
  std::string                                      channel_fading_distribution      = "uniform-phase";
  float                                            sinr_dB                          = 60.0F;
  float                                            cfo_Hz                           = 0.0F;
  unsigned                                         nof_corrupted_re_per_ofdm_symbol = 0;
  unsigned                                         nof_rx_ports                     = 2;
  unsigned                                         nof_layers                       = 1;
  unsigned                                         bwp_size_rb                      = 273;
  pusch_mcs_table                                  mcs_table                        = pusch_mcs_table::qam64;
  sch_mcs_index                                    mcs_index                        = 20;
  bool                                             enable_dc_position               = false;
  std::string                                      pxsch_type                       = "auto";
  port_channel_estimator_td_interpolation_strategy td_interpolation_strategy =
      port_channel_estimator_td_interpolation_strategy::average;
  dmrs_additional_positions dmrs_additional_pos = dmrs_additional_positions::pos2;
  unsigned                  nof_ldpc_iterations = 10;
  std::vector<unsigned>     rep_rv_sequence     = {0};
  std::string               log_level           = "warning";
};

/// \brief Parse CLI11 arguments into a \c pxsch_bler_test_configuration.
///
/// \return The PxSCH BLER test configuration if the parsing succeds, otherwise \c std::nullopt.
std::optional<pxsch_bler_test_configuration> parse_configuration(int argc, char** argv);

} // namespace ocudu
