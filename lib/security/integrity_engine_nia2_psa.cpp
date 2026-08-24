// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "integrity_engine_nia2_psa.h"
#include "ocudu/security/security.h"

using namespace ocudu;
using namespace security;

#ifdef MBEDTLS_CMAC_C

integrity_engine_nia2_psa::integrity_engine_nia2_psa(sec_128_key        k_128_int_,
                                                     uint8_t            bearer_id_,
                                                     security_direction direction_,
                                                     bool               allow_unprotected_) :
  k_128_int(k_128_int_),
  bearer_id(bearer_id_),
  direction(direction_),
  logger(ocudulog::fetch_basic_logger("SEC")),
  allow_unprotected(allow_unprotected_)
{
  // TODO.
}

integrity_engine_nia2_psa::~integrity_engine_nia2_psa()
{
  // TODO.
}

security_status integrity_engine_nia2_psa::compute_mac(sec_mac& mac, const byte_buffer_view v, uint32_t count)
{
  // TODO.
  (void)bearer_id;
  (void)direction;
  return security_status::success;
}

security_status integrity_engine_nia2_psa::protect_integrity(byte_buffer& buf, uint32_t count)
{
  // TODO.
  return security_status::success;
}

security_status integrity_engine_nia2_psa::verify_integrity(byte_buffer& buf, uint32_t count)
{
  if (buf.length() <= sec_mac_len) {
    return security_status::integrity_failure;
  }

  byte_buffer_view v{buf, 0, buf.length() - sec_mac_len};
  byte_buffer_view m{buf, buf.length() - sec_mac_len, sec_mac_len};

  // Compute MAC-I.
  security::sec_mac mac    = {};
  security_status   status = compute_mac(mac, v, count);

  if (status != security_status::success) {
    return status;
  }

  // Verify MAC-I.
  if (!std::equal(mac.begin(), mac.end(), m.begin(), m.end())) {
    if (allow_unprotected) {
      // Unprotected PDUs are expected to fail the integrity check but must have zero MAC-I.
      static constexpr security::sec_mac zero_mac = {};
      if (std::equal(zero_mac.begin(), zero_mac.end(), m.begin(), m.end())) {
        // Integrity passed (as unprotected).
        logger.debug("Integrity check passed as unprotected with zero MAC-I. count={}", count);
        logger.debug("K_int: {}", k_128_int);
        logger.debug(v.begin(), v.end(), "Message input:");

        // Trim MAC-I from PDU.
        buf.trim_tail(sec_mac_len);
        return security_status::success_unprotected;
      }
    }
    // Integrity failure.
    security::sec_mac mac_rx;
    std::copy(m.begin(), m.end(), mac_rx.begin());
    logger.warning("Integrity check failed. count={}", count);
    logger.warning("K_int: {}", k_128_int);
    logger.warning("MAC-I received: {}", mac_rx);
    logger.warning("MAC-I expected: {}", mac);
    logger.warning(v.begin(), v.end(), "Message input:");
    return security_status::integrity_failure;
  }
  // Integrity passed (as protected).
  logger.debug("Integrity check passed. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC-I: {}", mac);
  logger.debug(v.begin(), v.end(), "Message input:");

  // Trim MAC-I from PDU.
  buf.trim_tail(sec_mac_len);
  return security_status::success;
}

#endif // MBEDTLS_CMAC_C
