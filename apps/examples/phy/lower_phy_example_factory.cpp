// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lower_phy_example_factory.h"
#include "ocudu/phy/lower/lower_phy_factory.h"

using namespace ocudu;

std::unique_ptr<lower_phy> ocudu::create_lower_phy(const lower_phy_configuration& config,
                                                   const lower_phy_dependencies&  deps)
{
  // Deduce frequency range from the subcarrier spacing.
  frequency_range fr = frequency_range::FR1;
  if (config.scs > subcarrier_spacing::kHz60) {
    fr = frequency_range::FR2;
  }

  // Create DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory();
  report_fatal_error_if_not(dft_factory, "Failed to create DFT factory.");

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                            = dft_factory;
  std::shared_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(modulator_factory, "Failed to create OFDM modulator factory.");

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(demodulator_factory, "Failed to create OFDM demodulator factory.");

  // Create OFDM PRACH demodulator factory.
  std::shared_ptr<ofdm_prach_demodulator_factory> prach_demodulator_factory =
      create_ofdm_prach_demodulator_factory_sw(dft_factory, config.srate, fr);

  // Create PDxCH processor factory.
  std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory =
      create_pdxch_processor_factory_sw(modulator_factory, config.gain_backoff_dB);
  report_fatal_error_if_not(pdxch_proc_factory, "Failed to create PDxCH processor factory.");

  // Create PRACH processor factory.
  std::shared_ptr<prach_processor_factory> prach_proc_factory = create_prach_processor_factory_sw(
      prach_demodulator_factory, deps.prach_async_executor, config.srate, config.nof_rx_ports, 1);
  report_fatal_error_if_not(prach_proc_factory, "Failed to create PRACH processor factory.");

  // Create PUxCH processor factory.
  std::shared_ptr<puxch_processor_factory> puxch_proc_factory = create_puxch_processor_factory_sw(demodulator_factory);
  report_fatal_error_if_not(puxch_proc_factory, "Failed to create PUxCH processor factory.");

  // Create Downlink processor factory.
  std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory =
      create_downlink_processor_factory_sw(pdxch_proc_factory);
  report_fatal_error_if_not(downlink_proc_factory, "Failed to create downlink processor factory.");

  // Create Uplink processor factory.
  std::shared_ptr<lower_phy_uplink_processor_factory> uplink_proc_factory =
      create_uplink_processor_factory_sw(prach_proc_factory, puxch_proc_factory);
  report_fatal_error_if_not(uplink_proc_factory, "Failed to create uplink processor factory.");

  // Create Lower PHY factory.
  std::shared_ptr<lower_phy_factory> lphy_factory =
      create_lower_phy_factory_sw(downlink_proc_factory, uplink_proc_factory);
  report_fatal_error_if_not(lphy_factory, "Failed to create lower PHY factory.");

  return lphy_factory->create(config, deps);
}
