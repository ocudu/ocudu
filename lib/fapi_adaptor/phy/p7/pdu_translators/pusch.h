// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/ul_pusch_pdu.h"
#include "ocudu/phy/upper/uplink_pdu_slot_repository.h"

namespace ocudu {
namespace fapi_adaptor {

class uci_part2_correspondence_repository;

/// \brief Helper function that converts a PUSCH FAPI PDU into a PUSCH uplink slot PDU.
///
/// \param[out] pdu             Resulting PUSCH uplink slot PDU.
/// \param[in]  fapi_pdu        FAPI PUSCH PDU to convert.
/// \param[in]  slot            Slot in which the gNB receives the transmission.
/// \param[in]  num_rx_ant      Number of reception antennas.
/// \param[in]  part2_repo      UCI Part2 correspondence repository.
/// \param[in]  ntn_k_mac_slots Number of slots the gNB reception lags the UE transmission by.
void convert_pusch_fapi_to_phy(uplink_pdu_slot_repository::pusch_pdu& pdu,
                               const fapi::ul_pusch_pdu&              fapi_pdu,
                               slot_point                             slot,
                               uint16_t                               num_rx_ant,
                               uci_part2_correspondence_repository&   part2_repo,
                               unsigned                               ntn_k_mac_slots);

} // namespace fapi_adaptor
} // namespace ocudu
