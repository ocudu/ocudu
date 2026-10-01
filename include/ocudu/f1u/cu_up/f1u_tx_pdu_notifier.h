// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/nru/nru_message.h"

namespace ocudu {
namespace ocuup {

/// \brief This interface represents the NR-U exit point of the F1-U bearer of the CU-UP
/// through which it passes NR-U PDUs to lower layer (e.g. GTP-U) towards the DU.
class f1u_tx_pdu_notifier
{
public:
  virtual ~f1u_tx_pdu_notifier() = default;

  virtual void on_new_pdu(nru_dl_message msg) = 0;
};

} // namespace ocuup
} // namespace ocudu
