// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rx_resource_grid_printer_backend.h"
#include "ocudu/phy/upper/rx_symbol_printer_configuration.h"
#include "ocudu/phy/upper/upper_phy_rx_results_notifier.h"

namespace ocudu {

class upper_phy_rx_results_notifier_printer_decorator : public upper_phy_rx_results_notifier
{
public:
  explicit upper_phy_rx_results_notifier_printer_decorator(upper_phy_rx_results_notifier& base_notifier_,
                                                           std::shared_ptr<rx_resource_grid_printer_backend> backend_,
                                                           unsigned                                          sector_,
                                                           const rx_symbol_trigger_configuration& triggers_) :
    base_notifier(base_notifier_), backend(std::move(backend_)), sector(sector_), triggers(triggers_)
  {
  }

  // See the upper_phy_rx_results_notifier interface for documentation.
  void on_new_prach_results(const ul_prach_results& result) override
  {
    bool trigger =
        (!std::isnan(triggers.prach_threshold_rssi_dB) && (result.result.rssi_dB > triggers.prach_threshold_rssi_dB));

    if (trigger) {
      backend->on_prach_trigger(sector, result.context);
    }

    base_notifier.on_new_prach_results(result);
  }

  // See the upper_phy_rx_results_notifier interface for documentation.
  void on_new_pusch_results_control(const ul_pusch_results_control& result) override
  {
    base_notifier.on_new_pusch_results_control(result);
  }

  // See the upper_phy_rx_results_notifier interface for documentation.
  void on_new_pusch_results_data(const ul_pusch_results_data& result) override
  {
    bool                 trigger = triggers.pusch_on_ko && !result.decoder_result.tb_crc_ok;
    std::optional<float> sinr    = result.csi.get_sinr_dB();
    trigger                      = trigger || (!std::isnan(triggers.pusch_threshold_sinr_dB) && sinr.has_value() &&
                          (sinr.value() < triggers.pusch_threshold_sinr_dB));

    if (trigger) {
      backend->on_grid_trigger(sector, result.slot);
    }

    base_notifier.on_new_pusch_results_data(result);
  }

  // See the upper_phy_rx_results_notifier interface for documentation.
  void on_new_pucch_results(const ul_pucch_results& result) override { base_notifier.on_new_pucch_results(result); }

  // See the upper_phy_rx_results_notifier interface for documentation.
  void on_new_srs_results(const ul_srs_results& result) override { base_notifier.on_new_srs_results(result); }

private:
  upper_phy_rx_results_notifier&                    base_notifier;
  std::shared_ptr<rx_resource_grid_printer_backend> backend;
  unsigned                                          sector;
  rx_symbol_trigger_configuration                   triggers;
};
} // namespace ocudu
