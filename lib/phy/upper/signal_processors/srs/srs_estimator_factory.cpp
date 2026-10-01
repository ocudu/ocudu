// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_factory.h"
#include "logging_srs_estimator_decorator.h"
#include "srs_estimator_generic_impl.h"
#include "srs_estimator_pool.h"
#include "srs_validator_generic_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"

using namespace ocudu;

namespace {

class srs_estimator_factory_generic : public srs_estimator_factory
{
public:
  srs_estimator_factory_generic(std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory_,
                                std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory_,
                                std::shared_ptr<doa_estimator_factory>               doa_factory_,
                                unsigned                                             max_nof_prb_) :
    sequence_generator_factory(std::move(sequence_generator_factory_)),
    ta_estimator_factory(std::move(ta_estimator_factory_)),
    doa_factory(std::move(doa_factory_)),
    max_nof_prb(max_nof_prb_)
  {
    ocudu_assert(sequence_generator_factory, "Invalid sequence generator factory.");
    ocudu_assert(ta_estimator_factory, "Invalid TA estimator factory.");
    ocudu_assert(max_nof_prb != 0, "Maximum number of PRB cannot be zero.");

    // We don't check for doa_factory, since it's nullptr if DOA not configured.
  }

  std::unique_ptr<srs_estimator> create() override
  {
    srs_estimator_generic_impl::dependencies deps;
    deps.sequence_generator = sequence_generator_factory->create();
    deps.ta_estimator       = ta_estimator_factory->create();
    if (doa_factory) {
      deps.direction_estimator = doa_factory->create();
      // We assert the creation of the DOA estimator since it's an optional dependency of the SRS estimator.
      ocudu_assert(deps.direction_estimator, "DOA estimator couldn't be created.");
    }

    return std::make_unique<srs_estimator_generic_impl>(std::move(deps), max_nof_prb);
  }

  std::unique_ptr<srs_estimator_configuration_validator> create_validator() override
  {
    return std::make_unique<srs_validator_generic_impl>(max_nof_prb);
  }

private:
  std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory;
  std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory;
  std::shared_ptr<doa_estimator_factory>               doa_factory;
  unsigned                                             max_nof_prb;
};

class srs_estimator_factory_pool : public srs_estimator_factory
{
public:
  srs_estimator_factory_pool(std::shared_ptr<srs_estimator_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    ocudu_assert(factory, "Invalid factory.");
    ocudu_assert(nof_concurrent_threads != 0, "Number of threads must be larger than 0.");
  }

  // See interface for documentation.
  std::unique_ptr<srs_estimator> create() override
  {
    if (!pool) {
      std::vector<std::unique_ptr<srs_estimator>> estimators(nof_concurrent_threads);

      for (auto& estimator : estimators) {
        estimator = factory->create();
      }

      pool = std::make_shared<srs_estimator_pool::estimator_pool>(estimators);
    }

    return std::make_unique<srs_estimator_pool>(pool);
  }

  // See interface for documentation.
  std::unique_ptr<srs_estimator_configuration_validator> create_validator() override
  {
    return factory->create_validator();
  }

private:
  std::shared_ptr<srs_estimator_factory>              factory;
  std::shared_ptr<srs_estimator_pool::estimator_pool> pool;
  unsigned                                            nof_concurrent_threads;
};

} // namespace

std::shared_ptr<srs_estimator_factory> ocudu::create_srs_estimator_generic_factory(
    std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory,
    std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory,
    std::shared_ptr<doa_estimator_factory>               doa_factory,
    unsigned                                             max_nof_prb)
{
  return std::make_shared<srs_estimator_factory_generic>(
      std::move(sequence_generator_factory), std::move(ta_estimator_factory), std::move(doa_factory), max_nof_prb);
}

std::shared_ptr<srs_estimator_factory>
ocudu::create_srs_estimator_pool(std::shared_ptr<srs_estimator_factory> base_factory, unsigned max_nof_threads)
{
  return std::make_shared<srs_estimator_factory_pool>(std::move(base_factory), max_nof_threads);
}

std::unique_ptr<srs_estimator> srs_estimator_factory::create(ocudulog::basic_logger& logger)
{
  return std::make_unique<logging_srs_estimator_decorator>(logger, create());
}
