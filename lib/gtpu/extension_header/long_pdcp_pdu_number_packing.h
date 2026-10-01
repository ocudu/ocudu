// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu::gtpu {

/// \brief Packing and unpacking of GTP-U extension header 'Long PDCP PDU number'.
///
/// Ref: TS 29.281 Sec. 5.2.2.2A:
/// 'The PDCP PDU number field of the Long PDCP PDU number extension header has a maximum value which requires 18 bits.
/// (...) for direct Xn or indirect N3 DL data forwarding during a Handover procedure between two NG-RANs (...)'
///
/// Note: Packs and unpacks only the 'extension header content' field (TS 29.281 Sec 5.2.1 Fig 5.2.1-1).
/// Excludes leading octet for 'extension header length' and the trailing octet for 'next extension header type'.
class long_pdcp_pdu_number_packing
{
public:
  long_pdcp_pdu_number_packing(ocudulog::basic_logger& logger_) : logger(logger_) {}

  /// \brief Unpacks the extension header content of GTP-U extension header 'long PDCP PDU number'.
  ///
  /// \param[out] long_pdcp_pdu_number A 18-bit PDCP sequence number.
  /// \param[in] extension_header_content The packed extension header content.
  /// \return true on success, false otherwise.
  bool unpack(uint32_t& long_pdcp_pdu_number, byte_buffer_view extension_header_content) const;

  /// \brief Packs the extension header content of GTP-U extension header 'long PDCP PDU number'.
  ///
  /// \param[out] extension_header_content The packed extension header content.
  /// \param[in] long_pdcp_pdu_number A 18-bit PDCP sequence number.
  /// \return true on success, false otherwise.
  bool pack(byte_buffer& extension_header_content, const uint32_t long_pdcp_pdu_number) const;

private:
  ocudulog::basic_logger& logger;
};
} // namespace ocudu::gtpu
