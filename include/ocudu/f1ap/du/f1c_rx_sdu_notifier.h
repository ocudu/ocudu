// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu {
namespace odu {

/// \brief This interface represents the data exit point of the receiving side of a F1-C bearer of the DU.
/// The F1AP will push F1-C SDUs (e.g. PDCP PDUs/RLC SDUs) to the lower layer (e.g. RLC) using this interface.
class f1c_rx_sdu_notifier
{
public:
  virtual ~f1c_rx_sdu_notifier() = default;

  virtual void on_new_sdu(byte_buffer pdu) = 0;
};

} // namespace odu
} // namespace ocudu
