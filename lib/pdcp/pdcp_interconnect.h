// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/byte_buffer_chain.h"

namespace ocudu {

/// Status report interface of the PDCP RX entity
/// The PDCP TX entity uses this class to obtain a status report according to received and missing SDUs
class pdcp_rx_status_provider
{
public:
  virtual ~pdcp_rx_status_provider() = default;

  /// \brief Compiles a PDCP status report
  ///
  /// Ref: TS 38.323, Sec. 5.4.1, Sec. 6.2.3.1 and Sec. 6.3.{9,10}
  ///
  /// \return Control PDU for PDCP status report as a byte_buffer
  virtual byte_buffer compile_status_report() = 0;
};

/// This interface represents the status report entry point of a PDCP TX entity
/// The PDCP RX entity uses this class to forward received status reports to the TX entity
class pdcp_tx_status_handler
{
public:
  virtual ~pdcp_tx_status_handler() = default;

  /// \brief Status report handler for TX entity
  /// \param status The status PDU to be processed by the PDCP TX entity
  virtual void on_status_report(byte_buffer_chain status) = 0;
};

/// This interface represents the entry point of a PDCP TX entity for handling of ROHC feedback.
/// The PDCP RX entity uses this class to forward received and produced ROHC feedback to the TX entity.
class pdcp_tx_feedback_handler
{
public:
  virtual ~pdcp_tx_feedback_handler() = default;

  /// \brief Handler for ROHC feedback received by the RX entity from the peer that shall be consumed by the TX entity.
  /// \param rohc_feedback The received ROHC feedback PDU to be processed by the ROHC compressor of the TX entity.
  virtual void on_rohc_feedback_received(byte_buffer_chain rohc_feedback) = 0;

  /// \brief Handler for ROHC feedback produced by the RX entity that shall be forwarded to the peer entity.
  /// \param rohc_feedback The produced ROHC feedback PDU to be forwarded by the TX entity.
  virtual void on_rohc_feedback_produced(byte_buffer rohc_feedback) = 0;
};

} // namespace ocudu
