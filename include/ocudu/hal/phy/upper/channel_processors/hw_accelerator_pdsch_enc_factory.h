// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include <memory>

namespace ocudu {
namespace hal {

/// PDSCH encoder hardware accelerator factory.
class hw_accelerator_pdsch_enc_factory
{
public:
  virtual ~hw_accelerator_pdsch_enc_factory()                = default;
  virtual std::unique_ptr<hw_accelerator_pdsch_enc> create() = 0;
};

} // namespace hal
} // namespace ocudu
