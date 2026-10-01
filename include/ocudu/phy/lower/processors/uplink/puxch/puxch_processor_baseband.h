// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class baseband_gateway_buffer_reader;
struct lower_phy_rx_symbol_context;

/// \brief Lower physical layer PUxCH processor - Baseband interface.
///
/// Processes baseband samples with OFDM symbol granularity. The OFDM symbol size is inferred from the slot numerology.
class puxch_processor_baseband
{
public:
  /// Default destructor.
  virtual ~puxch_processor_baseband() = default;

  /// \brief Processes a baseband OFDM symbol.
  ///
  /// \param[in] samples Baseband samples to process.
  /// \param[in] context OFDM Symbol context.
  /// \return \c true if the signal is processed, \c false otherwise.
  virtual bool process_symbol(const baseband_gateway_buffer_reader& samples,
                              const lower_phy_rx_symbol_context&    context) = 0;
};

} // namespace ocudu
