// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/security/integrity_engine.h"
#include "ocudu/security/security.h"

namespace ocudu {
namespace security {

class integrity_engine_generic final : public integrity_engine
{
public:
  integrity_engine_generic(sec_128_key         k_128_int_,
                           uint8_t             bearer_id_,
                           security_direction  direction_,
                           integrity_algorithm integ_algo_);
  ~integrity_engine_generic() = default;

  security_status protect_integrity(byte_buffer& buf, uint32_t count) override;
  security_status verify_integrity(byte_buffer& buf, uint32_t count) override;

private:
  sec_128_key         k_128_int;
  uint8_t             bearer_id;
  security_direction  direction;
  integrity_algorithm integ_algo;

  ocudulog::basic_logger& logger;
};

} // namespace security
} // namespace ocudu
