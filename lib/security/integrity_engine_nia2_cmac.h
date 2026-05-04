// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h" // Nested include of MBEDTLS_CMAC_C from either config.h (v2) or mbedtls_config.h (v3)
#include "ocudu/security/integrity_engine.h"
#include "ocudu/security/security.h"

namespace ocudu {
namespace security {

#ifdef MBEDTLS_CMAC_C

class integrity_engine_nia2_cmac final : public integrity_engine
{
public:
  integrity_engine_nia2_cmac(sec_128_key k_128_int_, uint8_t bearer_id_, security_direction direction_);
  ~integrity_engine_nia2_cmac();

  security_status protect_integrity(byte_buffer& buf, uint32_t count) override;
  security_status verify_integrity(byte_buffer& buf, uint32_t count) override;

private:
  security_status compute_mac(security::sec_mac& mac, const byte_buffer_view v, uint32_t count);

  sec_128_key        k_128_int;
  uint8_t            bearer_id;
  security_direction direction;

  ocudulog::basic_logger&      logger;
  const mbedtls_cipher_info_t* cipher_info;
  mbedtls_cipher_context_t     ctx;
};

#endif // MBEDTLS_CMAC_C

} // namespace security
} // namespace ocudu
