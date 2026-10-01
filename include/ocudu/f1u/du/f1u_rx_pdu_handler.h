// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/nru/nru_message.h"

namespace ocudu {
namespace odu {

/// \brief This interface represents the NR-U entry point of the receiving side of a F1-U bearer of the DU.
/// The upper layer (e.g. GTP-U) will use it to pass NR-U PDUs (from the CU-UP) into the F1-U bearer of the DU.
class f1u_rx_pdu_handler
{
public:
  virtual ~f1u_rx_pdu_handler() = default;

  virtual void handle_pdu(nru_dl_message msg) = 0;
};

} // namespace odu
} // namespace ocudu
