// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/security/integrity_engine.h"
#include "ocudu/security/security.h"
#include "ocudu/security/ssl.h"
#include <psa/crypto.h>

namespace ocudu::security {

#if OCUDU_MBEDTLS_PSA

class integrity_engine_nia2_psa final : public integrity_engine
{
public:
  integrity_engine_nia2_psa(sec_128_key        k_128_int_,
                            uint8_t            bearer_id_,
                            security_direction direction_,
                            bool               allow_unprotected_);
  ~integrity_engine_nia2_psa();

  security_status protect_integrity(byte_buffer& buf, uint32_t count) override;
  security_status verify_integrity(byte_buffer& buf, uint32_t count) override;

private:
  security_status compute_mac(security::sec_mac& mac, const byte_buffer_view v, uint32_t count);

  sec_128_key        k_128_int;
  uint8_t            bearer_id;
  security_direction direction;

  ocudulog::basic_logger& logger;
  bool                    allow_unprotected = false;

  aes_context ctx;
};

#endif

} // namespace ocudu::security
