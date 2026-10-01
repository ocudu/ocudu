// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/memory_pool/ring_buffer_pool.h"

namespace ocudu {

struct dl_paging_allocation;

/// Encodes Paging PDUs based on Paging information provided by the scheduler.
class paging_pdu_assembler
{
public:
  paging_pdu_assembler(ticking_ring_buffer_pool& pdu_pool_);
  ~paging_pdu_assembler();

  /// \brief Encode Paging MAC PDU from Paging grant information.
  /// \param[in] pg Paging grant information.
  /// \return Encoded Paging MAC PDU.
  span<const uint8_t> encode_paging_pdu(const dl_paging_allocation& pg);

private:
  class paging_message_buffer;

  /// Buffer pool holding Paging PDUs.
  ticking_ring_buffer_pool& pdu_pool;

  /// Local buffer storage to hold Paging PDUs.
  std::unique_ptr<paging_message_buffer> buffer;
};

} // namespace ocudu
