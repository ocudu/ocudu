// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_prach_buffer.h"

namespace ocudu {

struct prach_buffer_context;

/// \brief Lower physical layer PRACH processor - Notifier interface.
///
/// Notifies a variety of PRACH-related events such as requests outside the occasion window or overflows of PRACH
/// requests.
class prach_processor_notifier
{
public:
  /// Default destructor.
  virtual ~prach_processor_notifier() = default;

  /// \brief Notifies a PRACH request outside the slot window.
  ///
  /// See \ref lower_phy_error_notifier::on_prach_request_late for more information.
  /// \param[in] context PRACH context.
  virtual void on_prach_request_late(const prach_buffer_context& context) = 0;

  /// \brief Notifies an excess of PRACH requests.
  ///
  /// See \ref lower_phy_error_notifier::on_prach_request_late for more information.
  /// \param[in] context PRACH context.
  virtual void on_prach_request_overflow(const prach_buffer_context& context) = 0;

  /// \brief Notifies the completion of the PRACH request processing.
  ///
  /// See \ref lower_phy_error_notifier::on_prach_request_late for more information.
  /// \param[in] buffer  PRACH buffer.
  /// \param[in] context PRACH context.
  virtual void on_rx_prach_window(shared_prach_buffer buffer, const prach_buffer_context& context) = 0;
};

} // namespace ocudu
