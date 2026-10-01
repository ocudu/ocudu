// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/channel_processors/pdcch/factories.h"
#include "logging_pdcch_processor_decorator.h"
#include "pdcch_encoder_impl.h"
#include "pdcch_modulator_impl.h"
#include "pdcch_processor_impl.h"
#include "pdcch_processor_pool.h"
#include "pdcch_processor_validator_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

namespace {

class pdcch_modulator_factory_sw : public pdcch_modulator_factory
{
private:
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory;

public:
  pdcch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                             std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory_) :
    modulator_factory(std::move(modulator_factory_)),
    prg_factory(std::move(prg_factory_)),
    rg_mapper_factory(std::move(rg_mapper_factory_))
  {
    ocudu_assert(modulator_factory, "Invalid modulator factory.");
    ocudu_assert(prg_factory, "Invalid PRG factory.");
    ocudu_assert(rg_mapper_factory, "Invalid resource grid mapper factory.");
  }

  std::unique_ptr<pdcch_modulator> create() override
  {
    return std::make_unique<pdcch_modulator_impl>(
        modulator_factory->create(), prg_factory->create(), rg_mapper_factory->create());
  }
};

class pdcch_encoder_factory_sw : public pdcch_encoder_factory
{
private:
  std::shared_ptr<crc_calculator_factory> crc_factory;
  std::shared_ptr<polar_factory>          polar_code_factory;

public:
  pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory_,
                           std::shared_ptr<polar_factory>          polar_code_factory_) :
    crc_factory(std::move(crc_factory_)), polar_code_factory(std::move(polar_code_factory_))
  {
    ocudu_assert(crc_factory, "Invalid CRC calculator factory.");
    ocudu_assert(polar_code_factory, "Invalid Polar code factory.");
  }

  std::unique_ptr<pdcch_encoder> create() override
  {
    return std::make_unique<pdcch_encoder_impl>(crc_factory->create(crc_generator_poly::CRC24C),
                                                polar_code_factory->create_interleaver(),
                                                polar_code_factory->create_allocator(),
                                                polar_code_factory->create_code(),
                                                polar_code_factory->create_encoder(),
                                                polar_code_factory->create_rate_matcher());
  }
};

class pdcch_processor_factory_sw : public pdcch_processor_factory
{
private:
  std::shared_ptr<pdcch_encoder_factory>        encoder_factory;
  std::shared_ptr<pdcch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory;

public:
  pdcch_processor_factory_sw(std::shared_ptr<pdcch_encoder_factory>        encoder_factory_,
                             std::shared_ptr<pdcch_modulator_factory>      modulator_factory_,
                             std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    modulator_factory(std::move(modulator_factory_)),
    dmrs_factory(std::move(dmrs_factory_))
  {
    ocudu_assert(encoder_factory, "Invalid encoder factory.");
    ocudu_assert(modulator_factory, "Invalid modulator factory.");
    ocudu_assert(dmrs_factory, "Invalid DM-RS factory.");
  }

  std::unique_ptr<pdcch_processor> create() override
  {
    return std::make_unique<pdcch_processor_impl>(
        encoder_factory->create(), modulator_factory->create(), dmrs_factory->create());
  }

  std::unique_ptr<pdcch_pdu_validator> create_validator() override
  {
    return std::make_unique<pdcch_processor_validator_impl>();
  }
};

class pdcch_processor_pool_factory : public pdcch_processor_factory
{
public:
  pdcch_processor_pool_factory(std::shared_ptr<pdcch_processor_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    ocudu_assert(factory, "Invalid PDCCH processor factory.");
    ocudu_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  std::unique_ptr<pdcch_processor> create() override
  {
    if (!processors) {
      std::vector<std::unique_ptr<pdcch_processor>> procs(nof_concurrent_threads);
      std::generate(procs.begin(), procs.end(), [this]() { return factory->create(); });
      processors = std::make_shared<pdcch_processor_pool::pdcch_processor_pool_type>(procs);
    }

    return std::make_unique<pdcch_processor_pool>(std::move(processors));
  }

  std::unique_ptr<pdcch_processor> create(ocudulog::basic_logger& logger, bool enable_logging_broadcast) override
  {
    if (!processors) {
      std::vector<std::unique_ptr<pdcch_processor>> procs(nof_concurrent_threads);
      std::generate(procs.begin(), procs.end(), [this, &logger, enable_logging_broadcast]() {
        return factory->create(logger, enable_logging_broadcast);
      });
      processors = std::make_shared<pdcch_processor_pool::pdcch_processor_pool_type>(procs);
    }

    return std::make_unique<pdcch_processor_pool>(std::move(processors));
  }

  std::unique_ptr<pdcch_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<pdcch_processor_factory>                         factory;
  unsigned                                                         nof_concurrent_threads;
  std::shared_ptr<pdcch_processor_pool::pdcch_processor_pool_type> processors;
};

} // namespace

std::shared_ptr<pdcch_encoder_factory>
ocudu::create_pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory,
                                       std::shared_ptr<polar_factory>          encoder_factory)
{
  return std::make_shared<pdcch_encoder_factory_sw>(std::move(crc_factory), std::move(encoder_factory));
}

std::shared_ptr<pdcch_modulator_factory>
ocudu::create_pdcch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory,
                                         std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                         std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory)
{
  return std::make_shared<pdcch_modulator_factory_sw>(
      std::move(modulator_factory), std::move(prg_factory), rg_mapper_factory);
}

std::shared_ptr<pdcch_processor_factory>
ocudu::create_pdcch_processor_factory_sw(std::shared_ptr<pdcch_encoder_factory>        encoder_factory,
                                         std::shared_ptr<pdcch_modulator_factory>      modulator_factory,
                                         std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory)
{
  return std::make_shared<pdcch_processor_factory_sw>(
      std::move(encoder_factory), std::move(modulator_factory), std::move(dmrs_factory));
}

std::shared_ptr<pdcch_processor_factory>
ocudu::create_pdcch_processor_pool_factory(std::shared_ptr<pdcch_processor_factory> processor_factory,
                                           unsigned                                 nof_concurrent_threads)
{
  return std::make_shared<pdcch_processor_pool_factory>(processor_factory, nof_concurrent_threads);
}

std::unique_ptr<pdcch_processor> pdcch_processor_factory::create(ocudulog::basic_logger& logger,
                                                                 bool                    enable_logging_broadcast)
{
  return std::make_unique<logging_pdcch_processor_decorator>(logger, enable_logging_broadcast, create());
}
