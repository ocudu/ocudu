// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_modulation/demodulation_mapper.h"
#include "ocudu/phy/upper/channel_modulation/evm_calculator.h"
#include "ocudu/phy/upper/channel_modulation/modulation_mapper.h"
#include <memory>

namespace ocudu {

class modulation_mapper_factory
{
public:
  virtual ~modulation_mapper_factory()                = default;
  virtual std::unique_ptr<modulation_mapper> create() = 0;
};

std::shared_ptr<modulation_mapper_factory> create_modulation_mapper_factory();

class demodulation_mapper_factory
{
public:
  virtual ~demodulation_mapper_factory()                = default;
  virtual std::unique_ptr<demodulation_mapper> create() = 0;
};

std::shared_ptr<demodulation_mapper_factory> create_demodulation_mapper_factory();

class evm_calculator_factory
{
public:
  virtual ~evm_calculator_factory()                = default;
  virtual std::unique_ptr<evm_calculator> create() = 0;
};

std::shared_ptr<evm_calculator_factory> create_evm_calculator_factory();

} // namespace ocudu
