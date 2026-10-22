// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#if OCUDU_HAVE_IPSEC_MB
#include "ocudu/security/integrity_engine.h"
#include "ocudu/security/security.h"
#include <ipsec-mb.h>

namespace ocudu::security {

class integrity_engine_nia2_ipsec_mb final : public integrity_engine
{
public:
  integrity_engine_nia2_ipsec_mb(sec_128_key        k_128_int_,
                                 uint8_t            bearer_id_,
                                 security_direction direction_,
                                 bool               allow_unprotected_);
  ~integrity_engine_nia2_ipsec_mb();

  security_status protect_integrity(byte_buffer& buf, uint32_t count) override;
  security_status verify_integrity(byte_buffer& buf, uint32_t count) override;

private:
  security_status compute_mac(security::sec_mac& mac, const byte_buffer_view v, uint32_t count);

  sec_128_key        k_128_int;
  uint8_t            bearer_id;
  security_direction direction;

  ocudulog::basic_logger& logger;
  bool                    allow_unprotected = false;

  IMB_MGR* mgr = nullptr;

  alignas(16) std::array<uint32_t, 60> expanded_enc_key{};
  alignas(16) std::array<uint32_t, 60> expanded_dec_key{};
  alignas(16) std::array<uint8_t, 16> subkey1{};
  alignas(16) std::array<uint8_t, 16> subkey2{};

  std::array<uint8_t, 9000> linear_buffer = {};
};

} // namespace ocudu::security

#endif
