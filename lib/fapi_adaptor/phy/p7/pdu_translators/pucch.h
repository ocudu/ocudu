// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/ul_pucch_pdu.h"
#include "ocudu/phy/upper/uplink_pdu_slot_repository.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Helper function that converts a PUCCH FAPI PDU into a PUCCH uplink slot PDU using the system frame number,
/// slot and number of reception antennas.
///
/// \param[out] pdu             Resulting PUCCH uplink slot PDU.
/// \param[in]  fapi_pdu        FAPI PUCCH PDU to convert.
/// \param[in]  slot            Slot in which the gNB receives the transmission.
/// \param[in]  num_rx_ant      Number of reception antennas.
/// \param[in]  ntn_k_mac_slots Number of slots the gNB reception lags the UE transmission by.
void convert_pucch_fapi_to_phy(uplink_pdu_slot_repository::pucch_pdu& pdu,
                               const fapi::ul_pucch_pdu&              fapi_pdu,
                               slot_point                             slot,
                               uint16_t                               num_rx_ant,
                               unsigned                               ntn_k_mac_slots);

} // namespace fapi_adaptor
} // namespace ocudu
