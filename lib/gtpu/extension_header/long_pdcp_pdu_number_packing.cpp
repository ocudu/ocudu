// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "long_pdcp_pdu_number_packing.h"
#include "ocudu/support/bit_encoding.h"

using namespace ocudu::gtpu;

/// Macro used to check a read/unpack operation and log an error message if the validation fails.
#define VERIFY_READ(cond)                                                                                              \
  if (!(cond)) {                                                                                                       \
    logger.error("Read failed in {} line {}", __FUNCTION__, __LINE__);                                                 \
    return false;                                                                                                      \
  }

/// Macro used to check a write/append/pack operation and log an error message if the validation fails.
#define VERIFY_WRITE(cond)                                                                                             \
  if (!(cond)) {                                                                                                       \
    logger.error("Write failed in {} line {}", __FUNCTION__, __LINE__);                                                \
    return false;                                                                                                      \
  }

bool long_pdcp_pdu_number_packing::unpack(uint32_t&        long_pdcp_pdu_number,
                                          byte_buffer_view extension_header_content) const
{
  if (extension_header_content.empty()) {
    logger.error("Failed to unpack long PDCP PDU number: pdu_len=0");
    return false;
  }

  if (extension_header_content.length() != 6) {
    logger.error("Failed to unpack long PDCP PDU number: pdu_len={} != 6", extension_header_content.length());
    return false;
  }

  bit_decoder decoder{extension_header_content};

  // Spare.
  uint32_t spare = 0;
  VERIFY_READ(decoder.unpack(spare, 6));
  if (spare != 0) {
    logger.warning("Spare bits set in octet 2. value={:#x}", spare);
    // TS 29.281 Sec. 5.2.2.2A
    // 'Bits 8 to 3 of octet 2 (...) shall be set to 0.'
    return false;
  }

  // Long PDCP PDU number.
  VERIFY_READ(decoder.unpack(long_pdcp_pdu_number, 18))

  // Spare.
  VERIFY_READ(decoder.unpack(spare, 24));
  if (spare != 0) {
    logger.warning("Spare bits set in octet 5 to 7. value={:#x}", spare);
    // TS 29.281 Sec. 5.2.2.2A
    // '(...) Bits 8 to 1 of octets 5 to 7 shall be set to 0.'
    return false;
  }

  return true;
}

bool long_pdcp_pdu_number_packing::pack(byte_buffer&   extension_header_content,
                                        const uint32_t long_pdcp_pdu_number) const
{
  if (long_pdcp_pdu_number > 0x3ffff) {
    logger.error("Cannot pack long PDCP PDU number that exceeds 18bits. pdcp_pdu_number={}", long_pdcp_pdu_number);
    return false;
  }

  bit_encoder encoder{extension_header_content};

  // Spare.
  VERIFY_WRITE(encoder.pack(0, 6));

  // Long PDCP PDU number.
  VERIFY_WRITE(encoder.pack(long_pdcp_pdu_number, 18));

  // Spare.
  VERIFY_WRITE(encoder.pack(0, 24));

  return true;
}
