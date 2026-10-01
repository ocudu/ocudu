// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_prach_buffer.h"

namespace ocudu {

struct prach_buffer_context;

/// \brief Lower physical layer PRACH processor - Request handler interface.
///
/// This class provides an interface for requesting the capture and demodulation of PRACH occasions.
class prach_processor_request_handler
{
public:
  /// Default destructor.
  virtual ~prach_processor_request_handler() = default;

  /// \brief Handles a new PRACH request.
  ///
  /// The PRACH buffer and context provided to this method are forwarded to a dedicated block that takes care of the
  /// request asynchronously. A notification through the \ref prach_processor_notifier interface will inform the client
  /// that the request has been processed and that the PRACH demodulated sequence is available in \c buffer.
  ///
  /// \param[in] buffer Destination PRACH buffer.
  /// \param[in] context PRACH context.
  virtual void handle_request(shared_prach_buffer buffer, const prach_buffer_context& context) = 0;
};

} // namespace ocudu
