// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "mbedtls/aes.h"
#include "ocudu/security/integrity_engine.h"
#include "ocudu/security/security.h"

namespace ocudu {
namespace security {

class integrity_engine_nia2_non_cmac final : public integrity_engine
{
public:
  integrity_engine_nia2_non_cmac(sec_128_key k_128_int_, uint8_t bearer_id_, security_direction direction_);
  ~integrity_engine_nia2_non_cmac() = default;

  security_status protect_integrity(byte_buffer& buf, uint32_t count) override;
  security_status verify_integrity(byte_buffer& buf, uint32_t count) override;

private:
  security_status compute_mac(security::sec_mac& mac, const byte_buffer_view v, uint32_t count);

  sec_128_key        k_128_int;
  uint8_t            bearer_id;
  security_direction direction;

  mbedtls_aes_context ctx;
  uint8_t             k1[16];
  uint8_t             k2[16];

  std::array<uint8_t, sec_max_pdu_size>    msg_buf;
  static constexpr std::array<uint8_t, 16> zeros = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  ocudulog::basic_logger& logger;
};

} // namespace security
} // namespace ocudu
