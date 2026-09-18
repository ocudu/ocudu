// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/ul_srs_pdu.h"
#include "ocudu/phy/upper/uplink_pdu_slot_repository.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Helper function that converts an SRS FAPI PDU into an SRS uplink slot PDU using the system frame number and
/// slot.
///
/// \param[out] pdu             Resulting SRS uplink slot PDU.
/// \param[in]  fapi_pdu        FAPI SRS PDU to convert.
/// \param[in]  sector_id_      Sector identifier.
/// \param[in]  nof_rx_antennas Number of reception antennas.
/// \param[in]  slot            Slot in which the gNB receives the transmission.
/// \param[in]  ntn_k_mac_slots Number of slots the gNB reception lags the UE transmission by.
void convert_srs_fapi_to_phy(uplink_pdu_slot_repository::srs_pdu& pdu,
                             const fapi::ul_srs_pdu&              fapi_pdu,
                             unsigned                             sector_id_,
                             unsigned                             nof_rx_antennas,
                             slot_point                           slot,
                             unsigned                             ntn_k_mac_slots);

} // namespace fapi_adaptor
} // namespace ocudu
