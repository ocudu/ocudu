// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_cp_types.h"

namespace ocudu::ocucp {

struct ngap_dl_nas_transport_message {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  byte_buffer      nas_pdu;
  bool             ue_cap_info_request = false;
};

} // namespace ocudu::ocucp
