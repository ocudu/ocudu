// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "ocudu/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_generator.h"

namespace ocudu {

class nzp_csi_rs_generator_factory
{
public:
  virtual ~nzp_csi_rs_generator_factory()                                        = default;
  virtual std::unique_ptr<nzp_csi_rs_generator>               create()           = 0;
  virtual std::unique_ptr<nzp_csi_rs_configuration_validator> create_validator() = 0;
  virtual std::unique_ptr<nzp_csi_rs_generator>               create(ocudulog::basic_logger& logger);
};

std::shared_ptr<nzp_csi_rs_generator_factory>
create_nzp_csi_rs_generator_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory);

std::shared_ptr<nzp_csi_rs_generator_factory>
create_nzp_csi_rs_generator_pool_factory(std::shared_ptr<nzp_csi_rs_generator_factory> generator_factory,
                                         unsigned                                      nof_concurrent_threads);

} // namespace ocudu
