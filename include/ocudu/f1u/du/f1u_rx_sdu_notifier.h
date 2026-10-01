// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu {
namespace odu {

/// \brief This interface represents the data exit point of the receiving side of a F1-U bearer of the DU.
/// The F1-U will push NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) to the lower layer (e.g. RLC) using this interface.
/// The F1-U will also inform the lower layer (e.g. RLC) of SDUs (e.g. PDCP PDUs) to be discarded.
class f1u_rx_sdu_notifier
{
public:
  virtual ~f1u_rx_sdu_notifier() = default;

  /// \brief Interface for lower layers to pass SDUs into RLC
  /// \param sdu_buf SDU to be handled
  /// \param is_retx Determines wheter the SDU is a PDCP retransmission or not
  virtual void on_new_sdu(byte_buffer sdu, bool is_retx) = 0;

  /// \brief Interface for lower layers to discard a block of SDUs from RLC queue.
  /// \param pdcp_sn_start PDCP sequence number (SN) of the first SDU in of the block that is to be discarded.
  /// \param block_size Number of consecutive PDCP SNs to be discarded.
  virtual void on_discard_sdu(uint32_t pdcp_sn_start, uint32_t block_size) = 0;
};

} // namespace odu
} // namespace ocudu
