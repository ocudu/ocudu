// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/security/security_engine.h"

namespace ocudu {
namespace security {

class ciphering_engine
{
public:
  virtual ~ciphering_engine() = default;

  virtual security_status apply_ciphering(byte_buffer& buf, size_t offset, uint32_t count) = 0;
};

} // namespace security
} // namespace ocudu
