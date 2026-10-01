// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/channel_processors/uci/uci_status.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/ran/sch/modulation_scheme.h"

namespace ocudu {

/// \brief Uplink Control Information decoder.
///
/// Decodes UCI payloads containing SR, HARQ-ACK and/or CSI bits, transmitted on PUCCH, reversing the steps described
/// in TS38.212 Sections 6.3.1.2, 6.3.1.3, 6.3.1.4 and 6.3.1.5.
///
/// Decodes UCI payloads of either HARQ-ACK or CSI bits, transmitted on PUSCH, reversing the steps described in
/// TS38.212 Sections 6.3.2.2, 6.3.2.3 and 6.3.2.4.
class uci_decoder
{
public:
  /// Collects UCI decoder configuration parameters.
  struct configuration {
    /// Transmission modulation.
    modulation_scheme modulation;
    // Add here other parameters.
    // ...
  };

  /// Default destructor.
  virtual ~uci_decoder() = default;

  /// \brief Decodes Uplink Control Information carried in either PUCCH or PUSCH.
  /// \param[out] message View of the decoded message.
  /// \param[in]  llr     The received soft bits, as a sequence of log-likelihood ratios.
  /// \param[in]  config  The UCI decoder configuration.
  /// \return The decoding status.
  virtual uci_status
  decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) = 0;
};

} // namespace ocudu
