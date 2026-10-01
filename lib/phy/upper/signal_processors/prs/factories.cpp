// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/prs/factories.h"
#include "logging_prs_generator_decorator.h"
#include "prs_generator_impl.h"
#include "prs_generator_pool_factory.h"
#include "prs_generator_validator_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

namespace {

/// Implements a generic PRS generator factory.
class prs_generator_factory_generic_impl : public prs_generator_factory
{
public:
  prs_generator_factory_generic_impl(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                     std::shared_ptr<channel_precoder_factory>        precoder_factory_) :
    prg_factory(std::move(prg_factory_)), precoder_factory(std::move(precoder_factory_))
  {
    ocudu_assert(prg_factory, "Invalid pseudo-random sequence generator factory.");
    ocudu_assert(precoder_factory, "Invalid channel precoder factory.");
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create() override
  {
    return std::make_unique<prs_generator_impl>(prg_factory->create(), precoder_factory->create());
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator_validator> create_validator() override
  {
    return std::make_unique<prs_generator_validator_impl>();
  }

private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<channel_precoder_factory>        precoder_factory;
};

} // namespace

std::unique_ptr<prs_generator> ocudu::prs_generator_factory::create(ocudulog::basic_logger& logger)
{
  return std::make_unique<logging_prs_generator_decorator>(logger, create());
}

std::shared_ptr<prs_generator_factory>
ocudu::create_prs_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                            std::shared_ptr<channel_precoder_factory>        precoder_factory)
{
  return std::make_shared<prs_generator_factory_generic_impl>(std::move(prg_factory), std::move(precoder_factory));
}

std::shared_ptr<prs_generator_factory>
ocudu::create_prs_generator_pool_factory(std::shared_ptr<prs_generator_factory> generator_factory,
                                         unsigned                               nof_concurrent_threads)
{
  return std::make_shared<prs_generator_pool_factory>(std::move(generator_factory), nof_concurrent_threads);
}
