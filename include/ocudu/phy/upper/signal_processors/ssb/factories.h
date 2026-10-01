// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "ocudu/phy/upper/signal_processors/ssb/dmrs_pbch_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/pss_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/sss_processor.h"

namespace ocudu {

class dmrs_pbch_processor_factory
{
public:
  virtual ~dmrs_pbch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pbch_processor> create() = 0;
};

std::shared_ptr<dmrs_pbch_processor_factory>
create_dmrs_pbch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

class pss_processor_factory
{
public:
  virtual ~pss_processor_factory()                = default;
  virtual std::unique_ptr<pss_processor> create() = 0;
};

std::shared_ptr<pss_processor_factory> create_pss_processor_factory_sw();

class sss_processor_factory
{
public:
  virtual ~sss_processor_factory()                = default;
  virtual std::unique_ptr<sss_processor> create() = 0;
};

std::shared_ptr<sss_processor_factory> create_sss_processor_factory_sw();

} // namespace ocudu
