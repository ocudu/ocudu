// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#ifdef OCUDU_HAVE_IPSEC_MB
#include "ipsec-mb.h"
#include "ocudu/security/ciphering_engine.h"
#include "ocudu/security/security.h"

namespace ocudu::security {

class ciphering_engine_nea2_ipsec_mb final : public ciphering_engine
{
public:
  ciphering_engine_nea2_ipsec_mb(sec_128_key k_128_enc_, uint8_t bearer_id_, security_direction direction_);
  ~ciphering_engine_nea2_ipsec_mb() override;

  security_status apply_ciphering(byte_buffer& buf, size_t offset, uint32_t count) override;

private:
  uint8_t            bearer_id;
  security_direction direction;
  sec_128_key        k_128_enc;

  ocudulog::basic_logger& logger;

  IMB_MGR* mgr = nullptr;

  alignas(16) std::array<uint32_t, 60> expanded_enc_key{};
  alignas(16) std::array<uint32_t, 60> expanded_dec_key{};

  std::array<uint8_t, 9000> linear_buffer = {};
};

} // namespace ocudu::security
#endif
