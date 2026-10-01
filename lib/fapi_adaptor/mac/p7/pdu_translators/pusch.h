// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/builders/ul_pusch_pdu_builder.h"

namespace ocudu {

struct ul_sched_info;

namespace fapi_adaptor {

class uci_part2_correspondence_mapper;

/// \brief Helper function that converts from a PUSCH MAC PDU to a PUSCH FAPI PDU.
///
/// \param[out] builder      PUSCH FAPI builder that helps to fill the PDU.
/// \param[in]  mac_pdu      MAC PDU that contains the PUSCH parameters.
/// \param[in]  part2_mapper UCI Part2 mapper.
void convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu_builder&      builder,
                               const ul_sched_info&             mac_pdu,
                               uci_part2_correspondence_mapper& part2_mapper);

} // namespace fapi_adaptor
} // namespace ocudu
