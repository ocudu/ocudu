// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class puxch_processor_notifier;
class puxch_processor_request_handler;
class puxch_processor_baseband;
class lower_phy_center_freq_controller;

/// \brief PUCCH and PUSCH processor main interface.
///
/// Provides a single access point to all PUCCH- and PUSCH-related interfaces, that is \ref puxch_processor_notifier,
/// \ref puxch_processor_request_handler and \ref puxch_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PUxCH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class puxch_processor
{
public:
  /// Default destructor.
  virtual ~puxch_processor() = default;

  /// Connects the PUxCH processor with a notifier.
  virtual void connect(puxch_processor_notifier& notifier) = 0;

  /// Stops the processor.
  virtual void stop() = 0;

  /// \brief Gets the PUxCH processor request handler.
  /// \return A reference to the internal PUxCH processor request handler.
  virtual puxch_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PUxCH processor baseband interface.
  /// \return A reference to the internal PUxCH processor baseband interface.
  virtual puxch_processor_baseband& get_baseband() = 0;

  /// Gets the carrier center frequency control interface.
  virtual lower_phy_center_freq_controller& get_center_freq_control() = 0;
};

} // namespace ocudu
