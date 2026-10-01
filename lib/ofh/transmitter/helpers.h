// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/transmitter/ofh_transmitter_configuration.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {
namespace ofh {

/// Returns the maximum value between the minimum T1a values in symbol units.
inline unsigned get_biggest_min_tx_parameter(const tx_window_timing_parameters& tx_timing_params)
{
  return std::max({tx_timing_params.sym_cp_dl_end, tx_timing_params.sym_cp_ul_end, tx_timing_params.sym_up_dl_end});
}

/// Returns duration of the OFH processing plus the transmission window in symbol units.
inline unsigned calculate_nof_symbols_before_ota(cyclic_prefix                     cp,
                                                 subcarrier_spacing                scs,
                                                 std::chrono::microseconds         processing_time,
                                                 const tx_window_timing_parameters tx_timing_params)
{
  unsigned nof_symbols = get_nsymb_per_slot(cp);
  auto     symbol_duration_ns =
      std::chrono::duration<double, std::nano>(1e6 / (nof_symbols * get_nof_slots_per_subframe(scs)));
  unsigned processing_time_in_symbols = std::max(1u, unsigned(std::floor(processing_time / symbol_duration_ns)));

  return processing_time_in_symbols + get_biggest_min_tx_parameter(tx_timing_params);
}

} // namespace ofh
} // namespace ocudu
