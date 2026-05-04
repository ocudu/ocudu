// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/security/ciphering_engine.h"
#include "ocudu/security/security.h"

namespace ocudu {
namespace security {

class ciphering_engine_nea3 final : public ciphering_engine
{
public:
  ciphering_engine_nea3(sec_128_key k_128_enc_, uint8_t bearer_id_, security_direction direction_);
  virtual ~ciphering_engine_nea3() = default;

  security_status apply_ciphering(byte_buffer& buf, size_t offset, uint32_t count) override;

private:
  sec_128_key        k_128_enc;
  uint8_t            bearer_id;
  security_direction direction;

  ocudulog::basic_logger& logger;
};

} // namespace security
} // namespace ocudu
