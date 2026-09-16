// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rrc_ue_logger.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/rrc/coarse_ue_location.h"
#include "ocudu/rrc/rrc_ue_capabilities.h"

namespace ocudu::ocucp {

class rrc_ue_context_update_notifier;

// Logging.
typedef enum { Rx = 0, Tx } direction_t;

template <class T>
void log_rrc_message(rrc_ue_logger&    logger,
                     const direction_t dir,
                     byte_buffer_view  pdu,
                     const T&          msg,
                     srb_id_t          srb_id,
                     const char*       msg_type);

// UE Capabilities.

rrc_ue_capabilities_t get_capabilities(asn1::rrc_nr::ue_nr_cap_s& ue_capabilities, rrc_ue_logger& logger);

std::optional<rrc_ue_capabilities_t> get_capabilities(asn1::rrc_nr::ue_cap_rat_container_list_l& capabilities_list,
                                                      rrc_ue_logger&                             logger);

/// \brief Decodes a coarse UE location and stores it in the UE context, TS 38.300 sec. 16.14.8.
///
/// The same Ellipsoid-Point reaches the gNB two ways: in a UEInformationResponse the gNB asked for, and in a
/// measurement report configured with coarseLocationRequest. Both land here. A position that differs from the one
/// held notifies the CU-CP, so a changed derived TAC reaches the AMF, TS 38.413 sec. 8.12.1.2.
void store_coarse_ue_location(std::optional<coarse_ue_location>& coarse_location,
                              const asn1::dyn_octstring&         coarse_location_info,
                              rrc_ue_context_update_notifier&    cu_cp_notifier,
                              rrc_ue_logger&                     logger);

} // namespace ocudu::ocucp
