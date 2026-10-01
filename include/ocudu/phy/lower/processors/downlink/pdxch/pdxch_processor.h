// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class pdxch_processor_notifier;
class pdxch_processor_baseband;
class pdxch_processor_request_handler;
class lower_phy_center_freq_controller;

/// \brief PDxCH processor main interface.
///
/// Provides a single access point to all downlink channel processor interfaces, that is \ref pdxch_processor_notifier,
/// \ref pdxch_processor_request_handler and \ref pdxch_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PDxCH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class pdxch_processor
{
public:
  /// Default destructor.
  virtual ~pdxch_processor() = default;

  /// Connects the PDxCH processor with a notifier.
  virtual void connect(pdxch_processor_notifier& notifier) = 0;

  /// \brief Gets the PDxCH processor request handler.
  /// \return A reference to the internal PDxCH processor request handler.
  virtual pdxch_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PDxCH processor baseband interface.
  /// \return A reference to the internal PDxCH processor baseband interface.
  virtual pdxch_processor_baseband& get_baseband() = 0;

  /// Gets the carrier center frequency control interface.
  virtual lower_phy_center_freq_controller& get_center_freq_control() = 0;

  /// Stops the processor.
  virtual void stop() = 0;
};

} // namespace ocudu
