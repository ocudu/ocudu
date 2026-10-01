// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_encoder.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_modulator.h"
#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "ocudu/phy/upper/signal_processors/pdcch/factories.h"
#include <memory>

namespace ocudu {

class pdcch_modulator_factory
{
public:
  virtual ~pdcch_modulator_factory()                = default;
  virtual std::unique_ptr<pdcch_modulator> create() = 0;
};

std::shared_ptr<pdcch_modulator_factory>
    create_pdcch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                      std::shared_ptr<pseudo_random_generator_factory>,
                                      std::shared_ptr<resource_grid_mapper_factory>);

class pdcch_encoder_factory
{
public:
  virtual ~pdcch_encoder_factory()                = default;
  virtual std::unique_ptr<pdcch_encoder> create() = 0;
};

std::shared_ptr<pdcch_encoder_factory>
create_pdcch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory,
                                std::shared_ptr<polar_factory>          encoder_factory);

class pdcch_processor_factory
{
public:
  virtual ~pdcch_processor_factory()                              = default;
  virtual std::unique_ptr<pdcch_processor>     create()           = 0;
  virtual std::unique_ptr<pdcch_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<pdcch_processor>     create(ocudulog::basic_logger& logger, bool enable_logging_broadcast);
};

std::shared_ptr<pdcch_processor_factory>
create_pdcch_processor_factory_sw(std::shared_ptr<pdcch_encoder_factory>        encoder_factory,
                                  std::shared_ptr<pdcch_modulator_factory>      modulator_factory,
                                  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_factory);

std::shared_ptr<pdcch_processor_factory>
create_pdcch_processor_pool_factory(std::shared_ptr<pdcch_processor_factory> processor_factory,
                                    unsigned                                 nof_concurrent_threads);

} // namespace ocudu
