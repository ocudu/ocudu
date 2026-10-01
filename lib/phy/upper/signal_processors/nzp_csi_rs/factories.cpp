// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/nzp_csi_rs/factories.h"
#include "nzp_csi_rs_generator_impl.h"
#include "nzp_csi_rs_generator_pool.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/support_formatters.h"
#include "ocudu/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_formatter.h"

using namespace ocudu;

namespace {

class nzp_csi_rs_generator_factory_sw : public nzp_csi_rs_generator_factory
{
public:
  nzp_csi_rs_generator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                  std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory_) :
    prg_factory(std::move(prg_factory_)), rg_mapper_factory(std::move(rg_mapper_factory_))
  {
    ocudu_assert(prg_factory, "Invalid PRG factory.");
    ocudu_assert(rg_mapper_factory, "Invalid resource grid mapper factory.");
  }
  std::unique_ptr<nzp_csi_rs_generator> create() override
  {
    return std::make_unique<nzp_csi_rs_generator_impl>(prg_factory->create(), rg_mapper_factory->create());
  }
  std::unique_ptr<nzp_csi_rs_configuration_validator> create_validator() override
  {
    return std::make_unique<nzp_csi_rs_configuration_validator_impl>();
  }

private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory;
};

class nzp_csi_rs_generator_pool_factory : public nzp_csi_rs_generator_factory
{
public:
  nzp_csi_rs_generator_pool_factory(std::shared_ptr<nzp_csi_rs_generator_factory> factory_,
                                    unsigned                                      nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    ocudu_assert(factory, "Invalid NZP-CSI-RS generator factory.");
    ocudu_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  std::unique_ptr<nzp_csi_rs_generator> create() override
  {
    if (!generators) {
      std::vector<std::unique_ptr<nzp_csi_rs_generator>> instances(nof_concurrent_threads);

      for (auto& processor : instances) {
        processor = factory->create();
      }

      generators = std::make_shared<nzp_csi_rs_generator_pool::generator_pool_type>(instances);
    }

    return std::make_unique<nzp_csi_rs_generator_pool>(generators);
  }

  std::unique_ptr<nzp_csi_rs_generator> create(ocudulog::basic_logger& logger) override
  {
    if (!generators) {
      std::vector<std::unique_ptr<nzp_csi_rs_generator>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this, &logger]() { return factory->create(logger); });
      generators = std::make_shared<nzp_csi_rs_generator_pool::generator_pool_type>(instances);
    }

    return std::make_unique<nzp_csi_rs_generator_pool>(generators);
  }

  std::unique_ptr<nzp_csi_rs_configuration_validator> create_validator() override
  {
    return factory->create_validator();
  }

private:
  std::shared_ptr<nzp_csi_rs_generator_factory>                   factory;
  unsigned                                                        nof_concurrent_threads;
  std::shared_ptr<nzp_csi_rs_generator_pool::generator_pool_type> generators;
};

class logging_nzp_csi_rs_generator_decorator : public nzp_csi_rs_generator
{
  template <typename Func>
  static std::chrono::nanoseconds time_execution(Func&& func)
  {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

public:
  logging_nzp_csi_rs_generator_decorator(ocudulog::basic_logger&               logger_,
                                         std::unique_ptr<nzp_csi_rs_generator> generator_) :
    logger(logger_), generator(std::move(generator_))
  {
    ocudu_assert(generator, "Invalid NZP CSI-RS generator.");
  }

  void map(resource_grid_writer& grid, const config_t& config) override
  {
    const auto&& func = [&]() { generator->map(grid, config); };

    std::chrono::nanoseconds time_ns = time_execution(func);

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all config fields.
      logger.debug(config.slot.sfn(), config.slot.slot_index(), "NZP-CSI: {:s} {}\n  {:n}", config, time_ns, config);
      return;
    }
    // Single line log entry.
    logger.info(config.slot.sfn(), config.slot.slot_index(), "NZP-CSI: {:s} {}", config, time_ns);
  }

private:
  ocudulog::basic_logger&               logger;
  std::unique_ptr<nzp_csi_rs_generator> generator;
};

} // namespace

std::shared_ptr<nzp_csi_rs_generator_factory>
ocudu::create_nzp_csi_rs_generator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                              std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory)
{
  return std::make_shared<nzp_csi_rs_generator_factory_sw>(std::move(prg_factory), std::move(rg_mapper_factory));
}

std::shared_ptr<nzp_csi_rs_generator_factory>
ocudu::create_nzp_csi_rs_generator_pool_factory(std::shared_ptr<nzp_csi_rs_generator_factory> generator_factory,
                                                unsigned                                      nof_concurrent_threads)
{
  return std::make_shared<nzp_csi_rs_generator_pool_factory>(std::move(generator_factory), nof_concurrent_threads);
}

std::unique_ptr<nzp_csi_rs_generator> nzp_csi_rs_generator_factory::create(ocudulog::basic_logger& logger)
{
  return std::make_unique<logging_nzp_csi_rs_generator_decorator>(logger, create());
}
