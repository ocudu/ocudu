// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/uci/uci_decoder.h"

namespace ocudu {

/// PUSCH UCI decoder interface to notify the decoding of a UCI message field.
class pusch_uci_decoder_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_uci_decoder_notifier() = default;

  /// \brief Notifies the completion of a PUSCH UCI message field decoding.
  /// \param[in] message decoded PUSCH UCI message.
  /// \param[in] status  UCI decoding status.
  virtual void on_uci_decoded(span<const uint8_t> message, const uci_status& status) = 0;
};

} // namespace ocudu
