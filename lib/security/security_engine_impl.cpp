// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "security_engine_impl.h"
#include "ciphering_engine_nea1.h"
#include "ciphering_engine_nea2.h"
#include "ciphering_engine_nea3.h"
#include "integrity_engine_generic.h"
#include "integrity_engine_nia2_cmac.h"
#include "integrity_engine_nia2_non_cmac.h"

using namespace ocudu;
using namespace security;

security_engine_impl::security_engine_impl(security::sec_128_as_config sec_cfg,
                                           uint8_t                     bearer_id,
                                           security_direction          direction,
                                           security::integrity_enabled integrity_enabled,
                                           security::ciphering_enabled ciphering_enabled) :
  logger(ocudulog::fetch_basic_logger("SEC"))
{
  bool allow_unprotected = integrity_enabled == security::integrity_enabled::smc_transition;
  if (integrity_enabled != security::integrity_enabled::off) {
    ocudu_assert(sec_cfg.integ_algo.has_value(), "Cannot enable integrity protection: No algorithm selected");
    ocudu_assert(sec_cfg.k_128_int.has_value(), "Cannot enable integrity protection: No key");
    switch (sec_cfg.integ_algo.value()) {
      case integrity_algorithm::nia2:
#ifdef MBEDTLS_CMAC_C
        integ_eng = std::make_unique<integrity_engine_nia2_cmac>(
            sec_cfg.k_128_int.value(), bearer_id, direction, allow_unprotected);
#else
        integ_eng = std::make_unique<integrity_engine_nia2_non_cmac>(
            sec_cfg.k_128_int.value(), bearer_id, direction, allow_unprotected);
#endif
        break;
      default:
        integ_eng = std::make_unique<integrity_engine_generic>(
            sec_cfg.k_128_int.value(), bearer_id, direction, sec_cfg.integ_algo.value(), allow_unprotected);
        break;
    }
  }
  if (ciphering_enabled == security::ciphering_enabled::on) {
    switch (sec_cfg.cipher_algo) {
      case ciphering_algorithm::nea1:
        cipher_eng = std::make_unique<ciphering_engine_nea1>(sec_cfg.k_128_enc, bearer_id, direction);
        break;
      case ciphering_algorithm::nea2:
        cipher_eng = std::make_unique<ciphering_engine_nea2>(sec_cfg.k_128_enc, bearer_id, direction);
        break;
      case ciphering_algorithm::nea3:
        cipher_eng = std::make_unique<ciphering_engine_nea3>(sec_cfg.k_128_enc, bearer_id, direction);
        break;
      default:
        // no cipher_eng for NEA0
        break;
    }
  }
}

security_status security_engine_impl::encrypt_and_protect_integrity(byte_buffer& buf, size_t offset, uint32_t count)
{
  security_status integ_status = security_status::success_unprotected;

  // apply integrity protection if activated
  if (integ_eng != nullptr) {
    integ_status = integ_eng->protect_integrity(buf, count);
    if (not is_success(integ_status)) {
      return integ_status;
    }
  }

  // apply ciphering if activated
  if (cipher_eng != nullptr) {
    security_status cipher_status = cipher_eng->apply_ciphering(buf, offset, count);
    if (not is_success(cipher_status)) {
      return cipher_status;
    }
  }

  // return the integrity success type (protected/unprotected)
  return integ_status;
}

security_status security_engine_impl::decrypt_and_verify_integrity(byte_buffer& buf, size_t offset, uint32_t count)
{
  // apply deciphering if activated
  if (cipher_eng != nullptr) {
    security_status cipher_status = cipher_eng->apply_ciphering(buf, offset, count);
    if (not is_success(cipher_status)) {
      return cipher_status;
    }
  }

  security_status integ_status = security_status::success_unprotected;

  // verify integrity if activated
  if (integ_eng != nullptr) {
    integ_status = integ_eng->verify_integrity(buf, count);
  }

  return integ_status;
}
