// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/phy/lower/modulation/ofdm_modulator.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// Implements an OFDM modulator concurrent pool.
class ofdm_symbol_modulator_pool : public ofdm_symbol_modulator
{
public:
  /// Internal pool of modulators.
  using modulator_pool = bounded_unique_object_pool<ofdm_symbol_modulator>;

  /// \brief Constructs an OFDM symbol modulator concurrent pool.
  /// \param[in] base_       Base instance for getting the symbol size.
  /// \param[in] modulators_ Shared pool of OFDM modulators..
  ofdm_symbol_modulator_pool(std::unique_ptr<ofdm_symbol_modulator> base_,
                             std::shared_ptr<modulator_pool>        modulators_) :
    logger(ocudulog::fetch_basic_logger("PHY")), base(std::move(base_)), modulators(std::move(modulators_))
  {
  }

  // See the interface for documentation.
  unsigned get_symbol_size(unsigned symbol_index) const override { return base->get_symbol_size(symbol_index); }

  // See the interface for documentation.
  void set_center_frequency(double center_frequency_Hz_) override
  {
    center_frequency_Hz.store(center_frequency_Hz_, std::memory_order_relaxed);
  }

  // See the interface for documentation.
  void modulate(span<cf_t>                  output,
                const resource_grid_reader& grid,
                span<const cf_t>            port_weights,
                unsigned                    symbol_index) override
  {
    auto modulator = modulators->get();
    if (!modulator) {
      ocuduvec::zero(output);
      logger.error("Insufficient number of OFDM modulator instances.");
      return;
    }

    double current_center_frequency_Hz = center_frequency_Hz.load(std::memory_order_relaxed);
    if (std::isnormal(current_center_frequency_Hz)) {
      modulator->set_center_frequency(current_center_frequency_Hz);
    }

    modulator->modulate(output, grid, port_weights, symbol_index);
  }

private:
  std::atomic<double>                    center_frequency_Hz = {};
  ocudulog::basic_logger&                logger;
  std::unique_ptr<ofdm_symbol_modulator> base;
  std::shared_ptr<modulator_pool>        modulators;
};

} // namespace ocudu
