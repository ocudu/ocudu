// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

///\file
///\brief pdcp_pdu_generator: Contains helper functions to generate dummy PDCP PDUs for testing purposes.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/pdcp/pdcp_sn_size.h"
#include <gtest/gtest.h>

namespace ocudu::test_helpers {

/// \brief Creates a byte_buffer serving as dummy PDCP PDU, e.g. for use as RLC SDU.
///
/// The produced PDU contains a PDCP data PDU header followed by an incremental sequence of bytes starting with the
/// value given by first_byte, i.e. if first_byte = 0xfc, the SDU will be <PDCP-HDR> 0xfc 0xfe 0xff 0x00 0x01 ...
/// The minimum sdu_size is 3 for 12-bit PDCP SNs and 4 for 18-bit PDCP SNs (i.e. PDCP-HDR + 1 or more bytes).
///
/// \param pdcp_sn_len Size of the PDCP sequence number
/// \param is_srb Determines the bearer type: SRB (true) or DRB (false).
/// \param pdcp_sn PDCP sequence number
/// \param sdu_size Size of the SDU (including PDCP header)
/// \param first_byte Value of the first payload byte after PDCP header
/// \return the produced SDU as a byte_buffer
byte_buffer
create_pdcp_pdu(pdcp_sn_size pdcp_sn_len, bool is_srb, uint32_t pdcp_sn, uint32_t sdu_size, uint8_t first_byte);

} // namespace ocudu::test_helpers
