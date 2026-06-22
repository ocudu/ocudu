// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// RX symbol print triggers.
struct rx_symbol_trigger_configuration {
  /// \brief PRACH buffer print trigger: RSSI threshold in decibels.
  ///
  /// Saves the PRACH buffer in the file when the RSSI is above the selected threshold. Set to infinity for not saving
  /// any PRACH buffer.
  float prach_threshold_rssi_dB = std::numeric_limits<float>::infinity();
  /// \brief UL resource grid print trigger: save the uplink grid when PUSCH message is not decoded successfully.
  ///
  /// Set to true for saving the uplink resource grid when a CRC check fails.
  bool pusch_on_ko = false;
  /// \brief UL resource grid print trigger: save the uplink grid when the measured SINR in a PUSCH message is below
  /// the threshold.
  ///
  /// Saves the uplink resource grid in the file when a PUSCH SINR for a slot is below the given threshold. Set to
  /// minus infinity for disabling this trigger.
  float pusch_threshold_sinr_dB = -std::numeric_limits<float>::infinity();
};

struct rx_symbol_printer_configuration {
  std::string                     filename;
  interval<unsigned>              ul_ports;
  rx_symbol_trigger_configuration triggers;
};

} // namespace ocudu
