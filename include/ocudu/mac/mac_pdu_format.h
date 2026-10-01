// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/logical_channel/lcid_dl_sch.h"

namespace ocudu {

constexpr unsigned FIXED_SIZED_MAC_CE_SUBHEADER_SIZE = 1;
constexpr unsigned ELCID_1_OCTET_FIELD_SIZE          = 1;
constexpr unsigned MAC_SDU_SUBHEADER_LENGTH_THRES    = 256;
constexpr unsigned MIN_MAC_SDU_SUBHEADER_SIZE        = 2;
constexpr unsigned MAX_MAC_SDU_SUBHEADER_SIZE        = 3;

/// \brief Derive the subheader size in bytes of a fixed-sized MAC CE as per TS 38.321, Section 6.1.2.
///
/// The subheader of a fixed-sized MAC CE carries no length field. It is one octet holding the LCID, or two when an
/// eLCID identifies the CE.
constexpr unsigned get_mac_ce_subheader_size(lcid_dl_sch_t lcid)
{
  return FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + (lcid.is_elcid() ? ELCID_1_OCTET_FIELD_SIZE : 0);
}

/// \brief Derive MAC SDU subheader size in bytes as per TS38.321, Section 6.1.
constexpr unsigned get_mac_sdu_subheader_size(unsigned payload)
{
  return payload == 0                                ? 0
         : payload >= MAC_SDU_SUBHEADER_LENGTH_THRES ? MAX_MAC_SDU_SUBHEADER_SIZE
                                                     : MIN_MAC_SDU_SUBHEADER_SIZE;
}

/// \brief Derive MAC SDU total size in bytes (includes subheader).
constexpr unsigned get_mac_sdu_required_bytes(unsigned payload)
{
  return payload + get_mac_sdu_subheader_size(payload);
}

/// \brief Derive MAC SDU payload size in bytes (without subheader) from the MAC SDU+subheader size.
constexpr unsigned get_mac_sdu_payload_size(unsigned mac_sdu_size)
{
  // Note: len(sdu)+len(subheader)==258 is impossible. If mac_sdu_size==258, we err on the side of lower payload and
  // return 255.
  if (mac_sdu_size <= MAC_SDU_SUBHEADER_LENGTH_THRES + MIN_MAC_SDU_SUBHEADER_SIZE - 1) {
    return mac_sdu_size - MIN_MAC_SDU_SUBHEADER_SIZE;
  }
  return mac_sdu_size - MAX_MAC_SDU_SUBHEADER_SIZE;
}

} // namespace ocudu
