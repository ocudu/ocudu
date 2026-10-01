// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "ocudu/phy/upper/channel_processors/ssb/pbch_encoder.h"
#include "ocudu/phy/upper/channel_processors/ssb/pbch_modulator.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/factories.h"

namespace ocudu {

class pbch_encoder_factory
{
public:
  virtual ~pbch_encoder_factory()                = default;
  virtual std::unique_ptr<pbch_encoder> create() = 0;
};

std::shared_ptr<pbch_encoder_factory>
create_pbch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                               std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                               std::shared_ptr<polar_factory>                   polar_factory);

class pbch_modulator_factory
{
public:
  virtual ~pbch_modulator_factory()                = default;
  virtual std::unique_ptr<pbch_modulator> create() = 0;
};

std::shared_ptr<pbch_modulator_factory>
    create_pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>,
                                     std::shared_ptr<pseudo_random_generator_factory>);

class ssb_processor_factory
{
public:
  virtual ~ssb_processor_factory()                              = default;
  virtual std::unique_ptr<ssb_processor>     create()           = 0;
  virtual std::unique_ptr<ssb_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<ssb_processor>     create(ocudulog::basic_logger& logger);
};

struct ssb_processor_factory_sw_configuration {
  std::shared_ptr<pbch_encoder_factory>        encoder_factory;
  std::shared_ptr<pbch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_factory;
  std::shared_ptr<pss_processor_factory>       pss_factory;
  std::shared_ptr<sss_processor_factory>       sss_factory;
};

std::shared_ptr<ssb_processor_factory> create_ssb_processor_factory_sw(ssb_processor_factory_sw_configuration& config);

std::shared_ptr<ssb_processor_factory>
create_ssb_processor_pool_factory(std::shared_ptr<ssb_processor_factory> processor_factory,
                                  unsigned                               nof_concurrent_threads);

} // namespace ocudu
