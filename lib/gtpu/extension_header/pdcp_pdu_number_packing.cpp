// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_pdu_number_packing.h"
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

bool pdcp_pdu_number_packing::unpack(uint32_t& pdcp_pdu_number, byte_buffer_view extension_header_content) const
{
  if (extension_header_content.empty()) {
    logger.error("Failed to unpack PDCP PDU number: pdu_len=0");
    return false;
  }

  if (extension_header_content.length() != 2) {
    logger.error("Failed to unpack PDCP PDU number: pdu_len={} != 2", extension_header_content.length());
    return false;
  }

  bit_decoder decoder{extension_header_content};

  // Spare.
  uint8_t spare = 0;
  VERIFY_READ(decoder.unpack(spare, 4));
  if (spare != 0) {
    logger.warning("Spare bits set in octet 2. value={:#x}", spare);
    // TS 29.281 Sec. 5.2.2.2
    // '(...) bits 5-8 of octet 2 are spare and shall be set to zero.'
    return false;
  }

  // PDCP PDU number.
  VERIFY_READ(decoder.unpack(pdcp_pdu_number, 12))

  return true;
}

bool pdcp_pdu_number_packing::pack(byte_buffer& extension_header_content, const uint32_t pdcp_pdu_number) const
{
  if (pdcp_pdu_number > 0xfff) {
    logger.error("Cannot pack PDCP PDU number that exceeds 12bits. pdcp_pdu_number={}", pdcp_pdu_number);
    return false;
  }

  bit_encoder encoder{extension_header_content};

  // Spare.
  VERIFY_WRITE(encoder.pack(0, 4));

  // PDCP PDU number.
  VERIFY_WRITE(encoder.pack(pdcp_pdu_number, 12));

  return true;
}
