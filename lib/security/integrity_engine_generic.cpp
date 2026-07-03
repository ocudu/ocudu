// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "integrity_engine_generic.h"
#include "ocudu/security/integrity.h"
#include "ocudu/security/security.h"

using namespace ocudu;
using namespace security;

integrity_engine_generic::integrity_engine_generic(sec_128_key         k_128_int_,
                                                   uint8_t             bearer_id_,
                                                   security_direction  direction_,
                                                   integrity_algorithm integ_algo_,
                                                   bool                allow_unprotected_) :
  k_128_int(k_128_int_),
  bearer_id(bearer_id_),
  direction(direction_),
  integ_algo(integ_algo_),
  logger(ocudulog::fetch_basic_logger("SEC")),
  allow_unprotected(allow_unprotected_)
{
}

security_status integrity_engine_generic::protect_integrity(byte_buffer& buf, uint32_t count)
{
  security::sec_mac mac = {};

  byte_buffer_view v{buf.begin(), buf.end()};

  logger.debug("Applying integrity protection. count={}", count);
  logger.debug(v.begin(), v.end(), "Message input:");

  switch (integ_algo) {
    case security::integrity_algorithm::nia0:
      // TS 33.501, Sec. D.1
      // The NIA0 algorithm shall be implemented in such way that it shall generate a 32 bit MAC-I/NAS-MAC and
      // XMAC-I/XNAS-MAC of all zeroes (see sub-clause D.3.1).
      std::fill(mac.begin(), mac.end(), 0);
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac, k_128_int, count, bearer_id, direction, v);
      break;
    default:
      break;
  }

  if (not buf.append(mac)) {
    logger.warning("Failed to append MAC-I to PDU. count={} pdu_len={}", count, buf.length());
    return security_status::buffer_failure;
  }

  logger.debug("Integrity protection applied. count={}", count);
  logger.debug("K_int: {}", k_128_int);
  logger.debug("MAC-I: {}", mac);
  logger.debug(buf.begin(), buf.end(), "Message output:");

  return security_status::success;
}

security_status integrity_engine_generic::verify_integrity(byte_buffer& buf, uint32_t count)
{
  security::sec_mac mac = {};

  if (buf.length() <= mac.size()) {
    return security_status::integrity_failure;
  }

  byte_buffer_view v{buf, 0, buf.length() - mac.size()};
  byte_buffer_view m{buf, buf.length() - mac.size(), mac.size()};

  switch (integ_algo) {
    case security::integrity_algorithm::nia0:
      // TS 33.501, Sec. D.1
      // The NIA0 algorithm shall be implemented in such way that it shall generate a 32 bit MAC-I/NAS-MAC and
      // XMAC-I/XNAS-MAC of all zeroes (see sub-clause D.3.1).
      std::fill(mac.begin(), mac.end(), 0);
      break;
    case security::integrity_algorithm::nia1:
      security_nia1(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia2:
      security_nia2(mac, k_128_int, count, bearer_id, direction, v);
      break;
    case security::integrity_algorithm::nia3:
      security_nia3(mac, k_128_int, count, bearer_id, direction, v);
      break;
    default:
      break;
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
    span m_rx{mac.data(), sec_mac_len};
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
  buf.trim_tail(mac.size());
  return security_status::success;
}
