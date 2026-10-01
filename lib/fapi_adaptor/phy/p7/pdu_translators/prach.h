// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/cell_config.h"
#include "ocudu/fapi/p7/messages/ul_tti_request.h"

namespace ocudu {

struct prach_buffer_context;
struct rach_config_common;

namespace fapi_adaptor {

/// \brief Helper function that converts from a UL PRACH FAPI PDU to a PRACH buffer context.
///
/// \param[out] context    Context for the PRACH buffer.
/// \param[in] fapi_pdu    FAPI PRACH PDU.
/// \param[in] prach_cfg   PRACH configuration as per SCF-222 v4.0 Section 3.3.2.4 TLV 0x1031.
/// \param[in] carrier_cfg Carrier configuration as per SCF-222 v4.0 Section 3.3.2.4 TLV 0x102d.
/// \param[in] ports       Port selection.
/// \param[in] sfn         System frame number.
/// \param[in] slot        Slot number.
/// \param[in] scs         Common subcarrier spacing.
/// \param[in] sector_id   Base station sector identifier.
void convert_prach_fapi_to_phy(prach_buffer_context&       context,
                               const fapi::ul_prach_pdu&   fapi_pdu,
                               const rach_config_common&   prach_cfg,
                               const fapi::carrier_config& carrier_cfg,
                               span<const uint8_t>         ports,
                               slot_point                  slot,
                               unsigned                    sector_id);

} // namespace fapi_adaptor
} // namespace ocudu
