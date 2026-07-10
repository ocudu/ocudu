// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "nrppa_asn1_converters.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/nrppa/nrppa.h"

namespace ocudu::ocucp {

// Logging
enum direction_t { Rx = 0, Tx };

void log_nrppa_message(ocudulog::basic_logger&          logger,
                       direction_t                      dir,
                       byte_buffer_view                 pdu,
                       const asn1::nrppa::nr_ppa_pdu_c& nrppa_pdu);

/// \brief Packs an NRPPA PDU, logs it as a Tx message, and forwards it to the CU-CP.
/// \param[in] logger The NRPPA logger.
/// \param[in] cu_cp_notifier The CU-CP notifier used to forward the packed PDU.
/// \param[in] pdu The NRPPA PDU to send.
/// \param[in] success_debug_name Debug name used when packing a successful outcome PDU.
/// \param[in] failure_debug_name Debug name used when packing an unsuccessful outcome PDU.
/// \param[in] dest_index The UE or AMF index to forward the packed PDU to.
template <typename DestIndexT>
void send_ul_nrppa_pdu(ocudulog::basic_logger&          logger,
                       nrppa_cu_cp_notifier&            cu_cp_notifier,
                       const asn1::nrppa::nr_ppa_pdu_c& pdu,
                       const char*                      success_debug_name,
                       const char*                      failure_debug_name,
                       DestIndexT                       dest_index)
{
  byte_buffer ul_nrppa_pdu =
      pack_into_pdu(pdu,
                    pdu.type().value == asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome ? success_debug_name
                                                                                                  : failure_debug_name);

  log_nrppa_message(logger, Tx, ul_nrppa_pdu, pdu);

  cu_cp_notifier.on_ul_nrppa_pdu(ul_nrppa_pdu, dest_index);
}

} // namespace ocudu::ocucp
