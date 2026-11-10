// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rrc_ue_logger.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/rrc/coarse_ue_location.h"
#include "ocudu/rrc/rrc_ue_capabilities.h"
#include <type_traits>

namespace ocudu::ocucp {

class rrc_ue_context_update_notifier;

// Logging.
typedef enum { Rx = 0, Tx } direction_t;

/// \brief Returns the name of the carried message, which a message class extension holds outside c1.
///
/// Reading c1 of a message class extension only logs an ASN.1 error and then reads the wrong member of the union, so
/// the alternative has to be checked before the name is taken.
template <class T>
const char* get_rrc_message_name(const T& msg)
{
  using msg_type_t = std::decay_t<decltype(msg.msg)>;
  if (msg.msg.type().value != msg_type_t::types_opts::c1) {
    return msg.msg.type().to_string();
  }
  return msg.msg.c1().type().to_string();
}

template <class T>
void log_rrc_message(rrc_ue_logger&    logger,
                     const direction_t dir,
                     byte_buffer_view  pdu,
                     const T&          msg,
                     srb_id_t          srb_id,
                     const char*       msg_type);

// UE Capabilities.

rrc_ue_capabilities_t get_capabilities(asn1::rrc_nr::ue_nr_cap_s& ue_capabilities, rrc_ue_logger& logger);

std::optional<rrc_ue_capabilities_t>
get_capabilities(const asn1::rrc_nr::ue_cap_rat_container_list_l& capabilities_list, rrc_ue_logger& logger);

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
