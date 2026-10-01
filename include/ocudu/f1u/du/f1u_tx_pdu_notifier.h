// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/nru/nru_message.h"

namespace ocudu {
namespace odu {

/// \brief This interface represents the NR-U exit point of a F1-U bearer of the DU.
/// The F1-U bearer will use this notifier to pass NR-U PDUs to upper layer (e.g. GTP-U) towards CU-UP.
class f1u_tx_pdu_notifier
{
public:
  virtual ~f1u_tx_pdu_notifier() = default;

  virtual void on_new_pdu(nru_ul_message msg) = 0;
};

} // namespace odu
} // namespace ocudu
