// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/nru/nru_message.h"

namespace ocudu {
namespace ocuup {

/// \brief This interface represents the NR-U entry point of a F1-U bearer of the CU-UP.
/// The lower layer (e.g. GTP-U) will use this class to pass NR-U PDUs (from the DU) into the F1-U.
class f1u_rx_pdu_handler
{
public:
  virtual ~f1u_rx_pdu_handler() = default;

  virtual void handle_pdu(nru_ul_message msg) = 0;
};

} // namespace ocuup
} // namespace ocudu
