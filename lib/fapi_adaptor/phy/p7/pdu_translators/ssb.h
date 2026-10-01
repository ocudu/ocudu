// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/dl_ssb_pdu.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Helper function that converts from a SSB FAPI PDU to a SSB processor PDU.
///
/// \param[out] proc_pdu SSB processor PDU.
/// \param[in] fapi_pdu FAPI PDU received that contains all the SSB parameters.
/// \param[in] sfn SFN when the FAPI PDU was received.
/// \param[in] slot Slot when the FAPI PDU was received.
/// \param[in] scs_common Parameter subCarrierSpacingCommon as per TS38.331, Section 6.2.2 \c MIB message.
void convert_ssb_fapi_to_phy(ssb_processor::pdu_t&   proc_pdu,
                             const fapi::dl_ssb_pdu& fapi_pdu,
                             slot_point              slot,
                             subcarrier_spacing      scs_common);

} // namespace fapi_adaptor
} // namespace ocudu
