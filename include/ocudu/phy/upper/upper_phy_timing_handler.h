// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct upper_phy_timing_context;

/// Upper physical layer handler to align timing boundaries.
class upper_phy_timing_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_timing_handler() = default;

  /// \brief Handles a new TTI boundary event.
  ///
  /// \param[in] context Notification context.
  virtual void handle_tti_boundary(const upper_phy_timing_context& context) = 0;

  /// \brief Handles a half slot (7th OFDM symbol) packet arrival.
  ///
  /// \param[in] context Notification context.
  virtual void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) = 0;

  /// \brief Handles a full slot (14th OFDM symbol) packet arrival.
  ///
  /// \param[in] context Notification context.
  virtual void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) = 0;
};

} // namespace ocudu
