// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Radio capability that allows modifying the transmitter gain at runtime.
class radio_tx_gain_control
{
public:
  virtual ~radio_tx_gain_control() = default;

  /// \brief Set the transmitter gain of a given port at runtime.
  ///
  /// \param[in] port_id Port identifier.
  /// \param[in] gain_dB New gain in dB.
  /// \return True if the operation is successful. Otherwise, false.
  /// \note The port identifier is indexed from stream ascending order and then port.
  virtual bool set_tx_gain(unsigned port_id, double gain_dB) = 0;
};

} // namespace ocudu
