// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/sdap/sdap_factory.h"
#include "sdap_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocuup;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<sdap_entity> ocudu::ocuup::create_sdap(sdap_entity_creation_message& msg)
{
  return std::make_unique<sdap_entity_impl>(msg.ue_index, msg.pdu_session_id, *msg.rx_sdu_notifier);
}
