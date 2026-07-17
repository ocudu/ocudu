// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/cell_configuration.h"
#include "ocudu/support/memory_pool/ring_buffer_pool.h"

namespace ocudu {

struct rar_information;
class mac_cell_rach_handler;

/// Encodes RAR PDUs based on RAR information provided by the scheduler.
class rar_pdu_assembler
{
public:
  /// \param pdu_pool Pool from which encoded RAR PDU buffers are allocated.
  /// \param rach_handler Used to resolve the UE Contention Resolution Identity of successRAR grants (see TS 38.321,
  /// 6.2.3a), decoded from the MsgA PUSCH CCCH SDU by the MAC UL PDU executor.
  rar_pdu_assembler(ticking_ring_buffer_pool& pdu_pool, mac_cell_rach_handler& rach_handler);

  /// Encodes a RAR PDU.
  span<const uint8_t> encode_rar_pdu(const rar_information& rar);

private:
  ticking_ring_buffer_pool& pdu_pool;
  mac_cell_rach_handler&    rach_handler;
};

} // namespace ocudu
