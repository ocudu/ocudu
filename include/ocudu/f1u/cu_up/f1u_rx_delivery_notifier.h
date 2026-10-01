// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {
namespace ocuup {

/// \brief This interface represents the notification exit point of the receiving side of a F1-U bearer of the CU-UP
/// through which the lower layer (e.g. RLC in the DU) notifies the PDCP Tx entity of transmit/delivery of PDCP PDUs.
class f1u_rx_delivery_notifier
{
public:
  virtual ~f1u_rx_delivery_notifier() = default;

  virtual void on_desired_buffer_size_notification(uint32_t desired_buffer_size) = 0;
  virtual void on_transmit_notification(uint32_t highest_pdcp_sn)                = 0;
  virtual void on_delivery_notification(uint32_t highest_pdcp_sn)                = 0;
  virtual void on_retransmit_notification(uint32_t highest_pdcp_sn)              = 0;
  virtual void on_delivery_retransmitted_notification(uint32_t highest_pdcp_sn)  = 0;
};

} // namespace ocuup
} // namespace ocudu
