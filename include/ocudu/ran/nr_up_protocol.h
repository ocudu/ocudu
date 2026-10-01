// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu {

/// NR user plane protocol uplink message as described in TS 38.425.
struct nr_up_ul_message {
  //: TODO: add here dl data delivery status fields of 38.425
  byte_buffer pdu;
};

/// NR user plane protocol downlink message as described in TS 38.425.
struct nr_up_dl_message {
  //: TODO: add here dl user data fields of 38.425
  byte_buffer pdu;
};

} // namespace ocudu
