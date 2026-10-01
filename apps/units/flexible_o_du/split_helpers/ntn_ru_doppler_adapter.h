// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_doppler_compensation_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <atomic>

namespace ocudu {

class ru_controller;

/// \brief NTN configuration manager - Radio Unit Doppler compensation adapter.
///
/// Applies the feeder link Doppler compensation computed by the NTN configuration manager of the DU-high to the RU
/// CFO controller. The RU is created after the DU and destroyed before it, so it is attached to this adapter once it
/// exists and detached before it goes away. Requests arriving outside that window are discarded.
class ntn_ru_doppler_adapter : public ocudu_ntn::ntn_doppler_compensation_handler
{
public:
  ntn_ru_doppler_adapter();

  // See interface for documentation.
  bool handle_dl_doppler_compensation(const ocudu_ntn::doppler_compensation_request& request) override;

  // See interface for documentation.
  bool handle_ul_doppler_compensation(const ocudu_ntn::doppler_compensation_request& request) override;

  /// Connects this adapter with the given RU controller.
  void connect(ru_controller& controller) { ru_ctrl.store(&controller, std::memory_order_release); }

  /// Disconnects this adapter from the RU controller. Called before the RU is destroyed.
  void disconnect() { ru_ctrl.store(nullptr, std::memory_order_release); }

private:
  ocudulog::basic_logger& logger;
  /// RU controller, accessed from the DU-high control executor and set from the thread driving the O-DU lifecycle.
  std::atomic<ru_controller*> ru_ctrl{nullptr};
};

} // namespace ocudu
