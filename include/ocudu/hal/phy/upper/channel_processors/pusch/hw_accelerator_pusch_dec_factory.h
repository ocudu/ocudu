// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include <memory>

namespace ocudu {
namespace hal {

/// PUSCH decoder hardware accelerator factory.
class hw_accelerator_pusch_dec_factory
{
public:
  virtual ~hw_accelerator_pusch_dec_factory()                = default;
  virtual std::unique_ptr<hw_accelerator_pusch_dec> create() = 0;
};

} // namespace hal
} // namespace ocudu
