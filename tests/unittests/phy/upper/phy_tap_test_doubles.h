// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/phy_tap/phy_tap.h"

namespace ocudu {

/// Implements a PHY tap interface for testing purposes.
class phy_tap_spy : public phy_tap
{
public:
  // See the phy_tap interface for documentation.
  void handle_ul_symbol(resource_grid_writer&                                     grid_writer,
                        const resource_grid_reader&                               grid_reader,
                        slot_point                                                slot,
                        unsigned                                                  symbol,
                        span<const uplink_pdu_slot_repository::pusch_pdu>         pusch_pdus,
                        span<const uplink_pdu_slot_repository::pucch_pdu>         pucch_pdus,
                        span<const pucch_processor::format1_common_configuration> pucch_f1_pdus,
                        span<const uplink_pdu_slot_repository::srs_pdu>           srs_pdus) override
  {
    ++handle_ul_symbol_count;
  }

  // See the phy_tap interface for documentation.
  void handle_prach_window(prach_buffer& buffer, const prach_buffer_context& context) override
  {
    ++handle_prach_window_count;
  }

  // See the phy_tap interface for documentation.
  void handle_quiet_grid(const resource_grid_reader& grid_reader, slot_point slot) override
  {
    ++handle_quiet_grid_count;
  }

  /// Gets the number of times handle_ul_symbol() is invoked.
  unsigned get_handle_ul_symbol_count() const { return handle_ul_symbol_count; }

  /// Gets the number of times handle_prach_window() is invoked.
  unsigned get_handle_prach_window_count() const { return handle_prach_window_count; }

  /// Gets the number of times handle_quiet_grid() is invoked.
  unsigned get_handle_quiet_grid_count() const { return handle_quiet_grid_count; }

private:
  /// Counts the number of times \ref handle_ul_symbol() is invoked.
  unsigned handle_ul_symbol_count = 0;
  /// Counts the number of times \ref handle_prach_window() is invoked.
  unsigned handle_prach_window_count = 0;
  /// Counts the number of times \ref handle_quiet_grid() is invoked.
  unsigned handle_quiet_grid_count = 0;
};

} // namespace ocudu
