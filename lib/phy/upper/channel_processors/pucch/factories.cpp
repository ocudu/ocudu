// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/channel_processors/pucch/factories.h"
#include "logging_pucch_processor_decorator.h"
#include "pucch_demodulator_impl.h"
#include "pucch_detector_format0.h"
#include "pucch_detector_format1.h"
#include "pucch_detector_impl.h"
#include "pucch_processor_impl.h"
#include "pucch_processor_pool.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include "ocudu/phy/upper/channel_processors/pucch/formatters.h"

using namespace ocudu;

namespace {

class pucch_detector_factory_sw : public pucch_detector_factory
{
private:
  std::shared_ptr<low_papr_sequence_collection_factory> low_papr_factory;
  std::shared_ptr<pseudo_random_generator_factory>      prg_factory;
  std::shared_ptr<channel_equalizer_factory>            eqzr_factory;
  std::shared_ptr<dft_processor_factory>                dft_factory;

public:
  pucch_detector_factory_sw(std::shared_ptr<low_papr_sequence_collection_factory> lpcf,
                            std::shared_ptr<pseudo_random_generator_factory>      prgf,
                            std::shared_ptr<channel_equalizer_factory>            eqzrf,
                            std::shared_ptr<dft_processor_factory>                dftf) :
    low_papr_factory(std::move(lpcf)),
    prg_factory(std::move(prgf)),
    eqzr_factory(std::move(eqzrf)),
    dft_factory(std::move(dftf))
  {
    ocudu_assert(low_papr_factory, "Invalid low-PAPR sequence collection factory.");
    ocudu_assert(prg_factory, "Invalid pseudorandom generator factory.");
    ocudu_assert(eqzr_factory, "Invalid channel equalizer factory.");
    ocudu_assert(dft_factory, "Invalid DFT processor factory.");
  }

  std::unique_ptr<pucch_detector> create() override
  {
    std::array<float, NOF_SUBCARRIERS_PER_RB> alphas;
    std::generate(alphas.begin(), alphas.end(), [n = 0U]() mutable {
      return (TWOPI * static_cast<float>(n++) / static_cast<float>(NOF_SUBCARRIERS_PER_RB));
    });

    std::unique_ptr<pucch_detector_format0> detector_format0 =
        std::make_unique<pucch_detector_format0>(prg_factory->create(), low_papr_factory->create(1, 0, alphas));

    std::unique_ptr<pucch_detector_format1> detector_format1 = std::make_unique<pucch_detector_format1>(
        low_papr_factory->create(1, 0, alphas),
        prg_factory->create(),
        dft_factory->create({.size = NOF_SUBCARRIERS_PER_RB, .dir = dft_processor::direction::DIRECT}),
        dft_factory->create({.size = NOF_SUBCARRIERS_PER_RB, .dir = dft_processor::direction::INVERSE}));

    return std::make_unique<pucch_detector_impl>(std::move(detector_format0), std::move(detector_format1));
  }
};

class pucch_processor_factory_sw : public pucch_processor_factory
{
public:
  pucch_processor_factory_sw(std::shared_ptr<dmrs_pucch_estimator_factory>&       dmrs_factory_,
                             std::shared_ptr<pucch_detector_factory>&             detector_factory_,
                             std::shared_ptr<pucch_demodulator_factory>           demodulator_factory_,
                             std::shared_ptr<uci_decoder_factory>                 decoder_factory_,
                             const channel_estimate::channel_estimate_dimensions& channel_estimate_dimensions_) :
    dmrs_factory(std::move(dmrs_factory_)),
    detector_factory(std::move(detector_factory_)),
    demodulator_factory(std::move(demodulator_factory_)),
    decoder_factory(std::move(decoder_factory_)),
    channel_estimate_dimensions(channel_estimate_dimensions_)
  {
    ocudu_assert(dmrs_factory, "Invalid DM-RS estimator factory.");
    ocudu_assert(detector_factory, "Invalid detector factory.");
    ocudu_assert(demodulator_factory, "Invalid PUCCH demodulator factory.");
    ocudu_assert(decoder_factory, "Invalid UCI decoder factory.");
    ocudu_assert((channel_estimate_dimensions.nof_symbols != 0) && (channel_estimate_dimensions.nof_prb != 0) &&
                     (channel_estimate_dimensions.nof_rx_ports != 0) &&
                     (channel_estimate_dimensions.nof_tx_layers != 0),
                 "Channel estimate dimensions cannot be zero.");
    ocudu_assert(channel_estimate_dimensions.nof_prb <= MAX_NOF_PRBS,
                 "Number of RB, i.e., {}, exceeds maximum bandwidth, i.e., {}.",
                 channel_estimate_dimensions.nof_prb,
                 MAX_NOF_PRBS);
    ocudu_assert(channel_estimate_dimensions.nof_symbols <= MAX_NSYMB_PER_SLOT,
                 "Number of OFDM symbols, i.e., {}, exceeds maximum slot symbols, i.e., {}.",
                 channel_estimate_dimensions.nof_symbols,
                 MAX_NSYMB_PER_SLOT);
    ocudu_assert(channel_estimate_dimensions.nof_rx_ports <= static_cast<unsigned>(MAX_PUCCH_RX_PORTS),
                 "Number of receive ports, i.e., {}, exceeds maximum supported receive ports, i.e., {}.",
                 channel_estimate_dimensions.nof_rx_ports,
                 static_cast<unsigned>(MAX_PUCCH_RX_PORTS));
    ocudu_assert(channel_estimate_dimensions.nof_tx_layers <= pucch_constants::NOF_LAYERS,
                 "Number of transmit layers, i.e., {}, exceeds maximum PUCCH transmit layers, i.e., {}.",
                 channel_estimate_dimensions.nof_tx_layers,
                 pucch_constants::NOF_LAYERS);
  }

  std::unique_ptr<pucch_processor> create() override
  {
    return std::make_unique<pucch_processor_impl>(create_validator(),
                                                  dmrs_factory->create(),
                                                  detector_factory->create(),
                                                  demodulator_factory->create(),
                                                  decoder_factory->create(),
                                                  channel_estimate_dimensions);
  }

  std::unique_ptr<pucch_pdu_validator> create_validator() override
  {
    return std::make_unique<pucch_pdu_validator_impl>(channel_estimate_dimensions);
  }

private:
  static constexpr unsigned                     MAX_PUCCH_RX_PORTS = 8;
  std::shared_ptr<dmrs_pucch_estimator_factory> dmrs_factory;
  std::shared_ptr<pucch_detector_factory>       detector_factory;
  std::shared_ptr<pucch_demodulator_factory>    demodulator_factory;
  std::shared_ptr<uci_decoder_factory>          decoder_factory;
  channel_estimate::channel_estimate_dimensions channel_estimate_dimensions;
};

class pucch_processor_pool_factory : public pucch_processor_factory
{
public:
  pucch_processor_pool_factory(std::shared_ptr<pucch_processor_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    ocudu_assert(factory, "Invalid PUCCH processor factory.");
    ocudu_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  std::unique_ptr<pucch_processor> create() override
  {
    if (!processors) {
      std::vector<std::unique_ptr<pucch_processor>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this]() { return factory->create(); });
      processors = std::make_shared<pucch_processor_pool::processor_pool>(instances);
    }
    return std::make_unique<pucch_processor_pool>(processors);
  }

  std::unique_ptr<pucch_processor> create(ocudulog::basic_logger& logger) override
  {
    if (!processors) {
      std::vector<std::unique_ptr<pucch_processor>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this, &logger]() { return factory->create(logger); });
      processors = std::make_shared<pucch_processor_pool::processor_pool>(instances);
    }
    return std::make_unique<pucch_processor_pool>(processors);
  }

  std::unique_ptr<pucch_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<pucch_processor_factory>              factory;
  unsigned                                              nof_concurrent_threads;
  std::shared_ptr<pucch_processor_pool::processor_pool> processors;
};

class pucch_demodulator_factory_sw : public pucch_demodulator_factory
{
public:
  std::unique_ptr<pucch_demodulator> create() override
  {
    std::unique_ptr<pucch_demodulator_format2> demodulator_format2 = std::make_unique<pucch_demodulator_format2>(
        equalizer_factory->create(), demodulation_factory->create(), prg_factory->create());

    std::unique_ptr<pucch_demodulator_format3> demodulator_format3 = std::make_unique<pucch_demodulator_format3>(
        equalizer_factory->create(), demodulation_factory->create(), prg_factory->create(), precoder_factory->create());

    std::unique_ptr<pucch_demodulator_format4> demodulator_format4 = std::make_unique<pucch_demodulator_format4>(
        equalizer_factory->create(), demodulation_factory->create(), prg_factory->create(), precoder_factory->create());

    return std::make_unique<pucch_demodulator_impl>(
        std::move(demodulator_format2), std::move(demodulator_format3), std::move(demodulator_format4));
  }

  pucch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory_,
                               std::shared_ptr<demodulation_mapper_factory>     demodulation_factory_,
                               std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                               std::shared_ptr<transform_precoder_factory>      precoder_factory_) :
    equalizer_factory(std::move(equalizer_factory_)),
    demodulation_factory(std::move(demodulation_factory_)),
    prg_factory(std::move(prg_factory_)),
    precoder_factory(std::move(precoder_factory_))
  {
    ocudu_assert(equalizer_factory, "Invalid equalizer factory.");
    ocudu_assert(demodulation_factory, "Invalid demodulation factory.");
    ocudu_assert(prg_factory, "Invalid PRG factory.");
    ocudu_assert(precoder_factory, "Invalid transform precoder factory.");
  }

private:
  std::shared_ptr<channel_equalizer_factory>       equalizer_factory;
  std::shared_ptr<demodulation_mapper_factory>     demodulation_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<transform_precoder_factory>      precoder_factory;
};

} // namespace

std::shared_ptr<pucch_processor_factory> ocudu::create_pucch_processor_factory_sw(
    std::shared_ptr<dmrs_pucch_estimator_factory>        dmrs_factory,
    std::shared_ptr<pucch_detector_factory>              detector_factory,
    std::shared_ptr<pucch_demodulator_factory>           demodulator_factory,
    std::shared_ptr<uci_decoder_factory>                 decoder_factory,
    const channel_estimate::channel_estimate_dimensions& channel_estimate_dimensions)
{
  return std::make_shared<pucch_processor_factory_sw>(
      dmrs_factory, detector_factory, demodulator_factory, decoder_factory, channel_estimate_dimensions);
}

std::shared_ptr<pucch_processor_factory>
ocudu::create_pucch_processor_pool_factory(std::shared_ptr<pucch_processor_factory> factory,
                                           unsigned                                 nof_concurrent_threads)
{
  return std::make_shared<pucch_processor_pool_factory>(std::move(factory), nof_concurrent_threads);
}

std::shared_ptr<pucch_demodulator_factory>
ocudu::create_pucch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory,
                                           std::shared_ptr<demodulation_mapper_factory>     demodulation_factory,
                                           std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                           std::shared_ptr<transform_precoder_factory>      precoder_factory)
{
  return std::make_shared<pucch_demodulator_factory_sw>(std::move(equalizer_factory),
                                                        std::move(demodulation_factory),
                                                        std::move(prg_factory),
                                                        std::move(precoder_factory));
}

std::shared_ptr<pucch_detector_factory>
ocudu::create_pucch_detector_factory_sw(std::shared_ptr<low_papr_sequence_collection_factory> lpcf,
                                        std::shared_ptr<pseudo_random_generator_factory>      prgf,
                                        std::shared_ptr<channel_equalizer_factory>            eqzrf,
                                        std::shared_ptr<dft_processor_factory>                dftf)
{
  return std::make_shared<pucch_detector_factory_sw>(
      std::move(lpcf), std::move(prgf), std::move(eqzrf), std::move(dftf));
}

std::unique_ptr<pucch_processor> pucch_processor_factory::create(ocudulog::basic_logger& logger)
{
  return std::make_unique<logging_pucch_processor_decorator>(logger, create());
}
