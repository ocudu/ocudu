// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_types.h"
#include "ocudu/sdap/sdap.h"
#include "ocudu/support/timers.h"
#include <memory>

/// This factory header file depends on the SDAP interfaces (see above include list). It is kept separate as clients of
/// the SDAP interfaces dont need to call factory methods.

namespace ocudu {

namespace ocuup {

struct sdap_entity_creation_message {
  uint32_t              ue_index;
  pdu_session_id_t      pdu_session_id;
  sdap_rx_sdu_notifier* rx_sdu_notifier;
};

/// Creates an instance of a SDAP interface.
std::unique_ptr<sdap_entity> create_sdap(sdap_entity_creation_message& msg);

} // namespace ocuup

} // namespace ocudu
