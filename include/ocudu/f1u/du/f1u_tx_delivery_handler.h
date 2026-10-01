// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {
namespace odu {

/// \brief This interface represents the notification entry point of the transmitting side of a F1-U bearer of the DU
/// through which the lower layer (e.g. RLC) notifies the PDCP Tx entity in the CU-UP of transmit/delivery of PDCP PDUs.
class f1u_tx_delivery_handler
{
public:
  virtual ~f1u_tx_delivery_handler() = default;

  /// \brief Handles a transmit notification from lower layers (i.e. from RLC UM/AM).
  ///
  /// Notification must be performed in ascending order of PDCP sequence numbers. It is the responsibility of the
  /// calling function (i.e. the RLC) to prevent out-of-order notifications.
  /// The parameter \c desired_buf_size informs the CU of the amount of data per DRB to be sent beyond the
  /// highest delivered PDCP SN (AM) or highest transmitted PDCP SN (UM).
  /// This corresponds to the total capacity of the RLC queue (in bytes).
  ///
  /// This function is quick and shall be called directly from pcell_executor to avoid excessive transitions across
  /// executors.
  ///
  /// Safe execution from: pcell_executor
  ///
  /// \param highest_pdcp_sn The highest transmitted PDCP sequence number
  /// \param desired_buf_size Desired buffer size for DRB
  virtual void handle_transmit_notification(uint32_t highest_pdcp_sn, uint32_t desired_buf_size) = 0;

  /// \brief Handles a delivery notification from lower layers (i.e. from RLC AM).
  ///
  /// Notification must be performed in ascending order of PDCP sequence numbers. It is the responsibility of the
  /// calling function (i.e. the RLC) to prevent out-of-order notifications.
  ///
  /// This function is quick and shall be called directly from pcell_executor to avoid excessive transitions across
  /// executors.
  ///
  /// Safe execution from: pcell_executor
  ///
  /// \param highest_pdcp_sn The highest delivered PDCP sequence number
  virtual void handle_delivery_notification(uint32_t highest_pdcp_sn) = 0;

  /// \brief Handles a retransmit notification from lower layers (i.e. from RLC AM).
  ///
  /// Notification must be performed in ascending order of PDCP sequence numbers. It is the responsibility of the
  /// calling function (i.e. the RLC) to prevent out-of-order notifications.
  ///
  /// This function is quick and shall be called directly from pcell_executor to avoid excessive transitions across
  /// executors.
  ///
  /// Safe execution from: pcell_executor
  ///
  /// \param highest_pdcp_sn The highest retransmitted PDCP sequence number
  virtual void handle_retransmit_notification(uint32_t highest_pdcp_sn) = 0;

  /// \brief Handles a delivery retransmitted notification from lower layers (i.e. from RLC AM).
  ///
  /// Notification must be performed in ascending order of PDCP sequence numbers. It is the responsibility of the
  /// calling function (i.e. the RLC) to prevent out-of-order notifications.
  ///
  /// This function is quick and shall be called directly from pcell_executor to avoid excessive transitions across
  /// executors.
  ///
  /// Safe execution from: pcell_executor
  ///
  /// \param highest_pdcp_sn The highest delivered retransmitted PDCP sequence number
  virtual void handle_delivery_retransmitted_notification(uint32_t highest_pdcp_sn) = 0;
};

} // namespace odu
} // namespace ocudu
