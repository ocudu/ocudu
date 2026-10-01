// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu {
namespace ocuup {

/// \brief This interface represents the data entry point of the transmitting side of a F1-U bearer of the CU-UP.
/// The upper layer (e.g. PDCP) will push NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) into the F1-U bearer towards the DU.
/// The upper layer (e.g. PDCP) will also inform the lower layer of SDUs (e.g. PDCP PDUs) to be discarded.
class f1u_tx_sdu_handler
{
public:
  virtual ~f1u_tx_sdu_handler() = default;

  /// \brief Immediately transmits a PDCP TX PDU to lower layers towards the DU.
  /// \param sdu The PDCP TX PDU to be transmitted to lower layers.
  /// \param is_retx Determines whether the SDU is a PDCP retransmission or not.
  virtual void handle_sdu(byte_buffer sdu, bool is_retx) = 0;

  /// \brief Enqueues a notification to discard the given PDCP TX PDU at the DU.
  ///
  /// The queued notifications are either piggy-backed with the next SDU or sent periodically (whatever comes first).
  /// \param pdcp_sn The sequence number of the PDCP PDU that shall be discarded by lower layers.
  virtual void discard_sdu(uint32_t pdcp_sn) = 0;
};

} // namespace ocuup
} // namespace ocudu
