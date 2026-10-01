// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer_chain.h"

namespace ocudu {
namespace ocuup {

/// \brief This interface represents the data exit point of the receiving side of a F1-U bearer of the CU-UP.
/// The F1-U bearer will use this notifier to pass NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) to upper layer (e.g. PDCP).
class f1u_rx_sdu_notifier
{
public:
  virtual ~f1u_rx_sdu_notifier() = default;

  virtual void on_new_sdu(byte_buffer_chain sdu) = 0;
};

} // namespace ocuup
} // namespace ocudu
