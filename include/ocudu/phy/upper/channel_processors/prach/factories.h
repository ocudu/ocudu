// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_generator.h"
#include <memory>

namespace ocudu {

class prach_generator_factory;
class task_executor;

class prach_detector_factory
{
public:
  virtual ~prach_detector_factory()                                    = default;
  virtual std::unique_ptr<prach_detector>           create()           = 0;
  virtual std::unique_ptr<prach_detector_validator> create_validator() = 0;
  virtual std::unique_ptr<prach_detector>           create(ocudulog::basic_logger& logger, bool log_all_opportunities);
};

struct prach_detector_factory_sw_configuration {
  unsigned idft_long_size    = 1024;
  unsigned idft_short_size   = 256;
  float    threshold_scaling = 1.0F;
};

std::shared_ptr<prach_detector_factory>
create_prach_detector_factory_sw(std::shared_ptr<dft_processor_factory>         dft_factory,
                                 std::shared_ptr<prach_generator_factory>       prach_gen_factory,
                                 const prach_detector_factory_sw_configuration& config = {});

std::shared_ptr<prach_detector_factory>
create_prach_detector_pool_factory(std::shared_ptr<prach_detector_factory> factory, unsigned nof_concurrent_threads);

class prach_generator_factory
{
public:
  virtual ~prach_generator_factory()                = default;
  virtual std::unique_ptr<prach_generator> create() = 0;
};

std::shared_ptr<prach_generator_factory> create_prach_generator_factory_sw();

} // namespace ocudu
