// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "logical_channel_system.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/drx_config.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

class cell_configuration;
class ue_cell_configuration;

/// Class that determines UE DRX active time.
class ue_drx_controller
{
public:
  ue_drx_controller(subcarrier_spacing                 scs_common,
                    std::chrono::milliseconds          conres_timer,
                    const std::optional<drx_config>&   drx_cfg,
                    ue_logical_channel_repository_view ul_lc_mng,
                    std::optional<slot_point>          ul_ccch_slot_rx,
                    ocudulog::basic_logger&            logger);

  /// Update UE DRX configuration.
  void reconfigure(const std::optional<drx_config>& new_drx_cfg);

  /// Update DRX controller state.
  void slot_indication(slot_point dl_slot);

  /// Determines whether the PDCCH can be allocated for a given slot.
  bool is_pdcch_enabled() const;

  /// Update DRX active time based on new DL PDCCH allocations.
  void on_new_dl_pdcch_alloc(slot_point pdcch_slot);

  /// Update DRX active time based on new UL PDCCH allocations.
  void on_new_ul_pdcch_alloc(slot_point pdcch_slot, slot_point pusch_slot);

  /// Update DRX active time based on a PUCCH HARQ NACK.
  void on_dl_harq_nack(slot_point uci_slot);

  /// Update DRX active time based on ContentionResolutionTimer.
  void on_con_res_start();

private:
  /// Whether the UE is within DRX active time.
  bool is_active_time() const;
  void update_inactivity_timer(slot_point pdcch_slot);

  const subcarrier_spacing           scs_common;
  std::chrono::milliseconds          conres_timer;
  ue_logical_channel_repository_view ul_lc_mng;
  std::optional<slot_point>          ul_ccch_slot_rx;
  ocudulog::basic_logger&            logger;

  // Current UE DRX config.
  std::optional<drx_config> drx_cfg;

  // Converted config parameters from milliseconds to slots.
  unsigned           active_window_period;
  interval<unsigned> active_window;
  unsigned           inactivity_dur;

  // End slot for the active window. When invalid, the UE is not in active window.
  slot_point active_time_end;
};

} // namespace ocudu
