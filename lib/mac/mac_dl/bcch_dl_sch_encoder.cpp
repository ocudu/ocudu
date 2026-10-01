// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bcch_dl_sch_encoder.h"

using namespace ocudu;

bcch_dl_sch_buffer ocudu::make_linear_bcch_dl_sch_buffer(const byte_buffer& pdu)
{
  auto vec = std::make_shared<std::vector<uint8_t>>(MAX_BCCH_DL_SCH_PDU_SIZE, 0);
  copy_segments(pdu, span<uint8_t>(vec->data(), vec->size()));
  return vec;
}
