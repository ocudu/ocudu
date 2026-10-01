// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/lower/processors/uplink/puxch/puxch_processor_factories.h"
#include "puxch_processor_impl.h"

using namespace ocudu;

namespace {

class puxch_processor_factory_sw : public puxch_processor_factory
{
public:
  explicit puxch_processor_factory_sw(std::shared_ptr<ofdm_demodulator_factory> ofdm_factory_) :
    ofdm_factory(std::move(ofdm_factory_))
  {
    ocudu_assert(ofdm_factory, "Invalid demodulator factory.");
  }

  std::unique_ptr<puxch_processor> create(const puxch_processor_configuration& config) override
  {
    // Calculate the DFT window offset in samples.
    unsigned nof_samples_window_offset =
        static_cast<unsigned>(static_cast<float>(config.cp.get_length(1, config.scs).to_samples(config.srate.to_Hz())) *
                              config.dft_window_offset);

    static constexpr unsigned nof_subcarriers_rb = 12;

    // Prepare OFDM demodulator configuration.
    ofdm_demodulator_configuration demodulator_config = {
        .numerology                = to_numerology_value(config.scs),
        .bw_rb                     = config.bandwidth_rb,
        .dft_size                  = config.srate.get_dft_size(config.scs),
        .cp                        = config.cp,
        .nof_samples_window_offset = nof_samples_window_offset,
        // Scale the DFT results to normalize the DFT output power.
        .scale          = 1.0F / static_cast<float>(std::sqrt(config.bandwidth_rb * nof_subcarriers_rb)),
        .center_freq_Hz = config.center_freq_Hz};

    // Prepare PUxCH processor configuration.
    puxch_processor_impl::configuration proc_config = {
        .cp = config.cp, .nof_rx_ports = config.nof_rx_ports, .dft_size = demodulator_config.dft_size};

    return std::make_unique<puxch_processor_impl>(ofdm_factory->create_ofdm_symbol_demodulator(demodulator_config),
                                                  proc_config);
  }

private:
  std::shared_ptr<ofdm_demodulator_factory> ofdm_factory;
};

} // namespace

std::shared_ptr<puxch_processor_factory>
ocudu::create_puxch_processor_factory_sw(std::shared_ptr<ofdm_demodulator_factory> ofdm_demod_factory)
{
  return std::make_shared<puxch_processor_factory_sw>(std::move(ofdm_demod_factory));
}
